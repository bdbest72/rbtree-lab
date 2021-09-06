#include "rbtree.h"

#include <malloc.h>

rbtree *new_rbtree(void)
{
  rbtree *p = (rbtree *)calloc(sizeof(rbtree), 1);
  return p;
}

void delete_rbtree(rbtree *t)
{
  // TODO: reclaim the tree nodes's memory
  free(t);
}

node_t *_Rotate(const key_t key, node_t *pivot, rbtree *t)
{
  node_t *child, *gchild;

  if ((key > pivot->key || key == pivot->key) && pivot != t->root)
    child = (node_t *)pivot->right;
  else
    child = (node_t *)pivot->left;
  if (key > child->key || key == child->key)
  {
    gchild = (node_t *)child->right;
    child->right = gchild->left;
    gchild->left = (node_t *)child;
  }
  else
  {
    gchild = (node_t *)child->left;
    child->left = gchild->right;
    gchild->right = (node_t *)child;
  }
  if ((key > pivot->key || key == pivot->key) && pivot != t->root)
    pivot->right = gchild;
  else
    pivot->left = gchild;

  return gchild;
}

node_t *rbtree_insert(rbtree *t, const key_t key)
{
  // TODO: implement insert

  // 첫 삽입
  if (t->root == NULL)
  {
    // 루트 만들기:
    //수정 01:
    node_t *temp;
    temp = malloc(sizeof(node_t));
    t->root = temp;
    t->root->key = key;
    t->root->color = RBTREE_BLACK;
    t->root->left = t->root->right = t->root->parent = NULL;

    return temp;
  }

  node_t *n, *p, *gp, *ggp;
  ggp = gp = p = t->root->parent;
  n = t->root->left;

  while (n)
  {
    if (key == n->key)
      return 0; //중복 방지

    if (n->left && n->right && n->left->color == RBTREE_RED && n->right->color == RBTREE_RED)
    {
      //하향 색변환 가능한 상황 자식 노드 두개가 빨강이고 나는 검정일때
      n->color = RBTREE_RED;
      n->left->color = n->right->color = RBTREE_BLACK;

      // 빨강이 연속되기에 회전 필요
      if (p->color = RBTREE_RED)
      {
        gp->color = RBTREE_RED;
        if ((key > gp->key) != (key > p->key))
          p = _Rotate(key, gp, t);
        n = _Rotate(key, ggp, t);
        n->color = RBTREE_BLACK;
      }

      // 루트는 언제나 검정
      t->root->color = RBTREE_BLACK;
    }

    //gp, p 등등 업데이트
    ggp = gp;
    gp = p;
    p = n;
    if (key > n->key)
      n = n->right;
    else
      n = n->left;
  }

  node_t *temp;
  temp = malloc(sizeof(node_t));
  temp->key = key;
  temp->left = temp->right = NULL;
  temp->parent = p;
  temp->color = RBTREE_RED;

  if (key > p->key && p != t->root)
    p->right = temp;
  else
    p->left = temp;

  if (p->color == RBTREE_RED)
  {
    gp->color = RBTREE_RED;
    if ((key > gp->key) != (key > p->key))
      p = _Rotate(key, gp, t);
    temp = _Rotate(key, ggp, t);
    temp->color = RBTREE_BLACK;
  }

  t->root->left->color = RBTREE_BLACK;

  return t->root;
}

node_t *rbtree_find(const rbtree *t, const key_t key)
{
  // TODO: implement find
  node_t *s;

  s = t->root->left;
  while (s && !(key == s->key))
  {
    if (key > s->key)
      s = s->right;
    else
      s = s->left;
  }
  if (s == 0)
    return 0;
  printf("찾았다! %d\n", s->key);
  return s;
}

node_t *rbtree_min(const rbtree *t)
{
  // TODO: implement find
  return t->root;
}

node_t *rbtree_max(const rbtree *t)
{
  // TODO: implement find
  return t->root;
}

int rbtree_erase(rbtree *t, node_t *p)
{
  // TODO: implement erase
  node_t *delgp, *delp, *del, *sib;

  int value = p->key;

  delgp = delp = t->root;
  del = t->root->left;
  sib = 0;
  while (!_IsLeafNode(del))
  {
    if (del->color != RBTREE_RED)
    {
      if (_RedAsParent(delgp, delp, sib, t))
      {
        delgp = sib;
        if (del->key > delp->key || del->key == delp->key)
          sib = delp->left;
        else
          sib = delp->right;
      }
    }

    if (del != t->root->left && _Is2Node(del))
    {
      if (!_BorrowKey(delgp, delp, del, sib, t))
        _BindNode(delp);
    }

    if (value == del->key)
      value = _Swapkey(del);

    delgp = delp;
    delp = del;
    if (value > del->key || value == del->key)
    {
      //swap key로 인해 같은 값이 나올수 있음.
      //In order successor을 사용하기 때문에 오른쪽으로
      sib = del->left;
      del = del->right;
    }
    else
    {
      sib = del->right;
      del = del->left;
    }
  }

  // while 종료 leaf 노드에 도달

  if (del->color != RBTREE_RED)
  {
    if (_RedAsParent(delgp, delp, sib, t))
    {
      // delgp 와 sib 의 위치가 변했다 새로 수정;
      delgp = sib;
      if (del->key > delp->key || del->key == delp->key)
        sib = del->left;
      else
        sib = delp->right;
    }
  }
  if (del != t->root->left && _Is2Node(del))
  {
    if (!_BorrowKey(delgp, delp, del, sib, t))
      _BindNode(delp);
  }
  if (_DelLeafNode(value, delp, del, t))
    return 1;
  else
    return 0;
}

// 삭제 함수에 들어가는 함수 1
int _IsLeafNode(const node_t *p)
{
  if (p == NULL)
    return 0;
  if ((p->left == NULL || (p->left && p->left->color == RBTREE_RED && !p->left->left && !p->left->right)) && (p->right == NULL || (p->right && p->right->color == RBTREE_RED && !p->right->left && !p->right->right)))
    return 1;
  else
    return 0;
}

// 삭제 함수에 들어가는 함수 2
int _RedAsParent(node_t *delgp, node_t *delp, node_t *sib, rbtree *t)
{
  if (sib == NULL || sib->color != RBTREE_RED)
    return 0;
  _Rotate(sib->key, delgp, t);
  sib->color = RBTREE_BLACK;
  delp->color = RBTREE_RED;
  return 1;
}

// 삭제 함수에 들어가는 함수 3
int _Is2Node(const node_t *p)
{
  if (p == NULL)
    return 0;
  if (p->color == RBTREE_RED)
    return 0;
  if ((p->left == NULL && p->right == NULL) || (p->left == RBTREE_RED && p->right == RBTREE_RED))
    return 1;
  else
    return 0;
}

// 삭제 함수에 들어가는 함수 4
int _BorrowKey(node_t *delgp, node_t *delp, node_t *del, node_t *sib, rbtree *t)
{
  node_t *sibrc;

  //2노드 일 경우 2노드 삭제를 진행하기 위해 탈출
  if (_Is2Node(sib))
    return 0;

  // del이 sib의 왼쪽인지 오른쪽인지 판별
  if (del->key > sib->key)
  {
    if (sib->left && sib->left->color == RBTREE_RED)
      sibrc = sib->left;
    else
      sibrc = sib->right;
  }
  else
  {
    if (sib->right && sib->right->color == RBTREE_RED)
      sibrc = sib->right;
    else
      sibrc = sib->left;
  }

  //그냥 회전인지 이중 회전인지
  if ((delp->key > sib->key) != (sib->key > sibrc->key))
  {
    //이중회전
    _Rotate(sibrc->key, delp, t);
    _Rotate(sibrc->key, delgp, t);
    sib->color = RBTREE_BLACK;
    sibrc->color = RBTREE_RED;
  }
  else
  {
    //단일 회전
    _Rotate(sib->key, delgp, t);
    sib->color = RBTREE_RED;
    sibrc->color = RBTREE_BLACK;
  }

  del->color = RBTREE_RED;
  delp->color = RBTREE_BLACK;

  if (t->root->left->color == RBTREE_RED)
    t->root->left->color = RBTREE_BLACK;
  return 1;
}

// 삭제 함수에 들어가는 함수 5
// 상향 색변환 / 형제와 결합
void _BindNode(node_t *delp)
{
  delp->color = RBTREE_BLACK;
  delp->left->color = RBTREE_RED;
  delp->right->color = RBTREE_RED;
}

// 삭제 함수에 들어가는 함수 6
key_t _Swapkey(node_t *del)
{
  node_t *cdd;
  cdd = del->right;
  while (cdd->left)
    cdd = cdd->left;
  del->key = cdd->key;
  return cdd->key;
}

// 삭제 함수에 들어가는 함수 7
int _DelLeafNode(const key_t key, node_t *delp, node_t *del, rbtree *t)
{
  if (key == del->key && !del->left && !del->right)
  {
    // red leaf 나 black leaf 인 경우
    free(del);
    if ((key > delp->key || key == delp->key) && delp != t->root)
      delp->right = NULL;
    else
      delp->left = NULL;
    return 1;
  }
  else if (key == del->key) //black 노드인 경우
  {
    node_t *ret;
    if (del->left)
    {
      del->left->right = del->right;
      ret = del->left;
      ret->color = RBTREE_BLACK;
      free(del);
    }
    else if (del->left)
    {
      del->right->left = del->left;
      ret = del->right;
      ret->color = RBTREE_BLACK;
      free(del);
    }
    if ((ret->key > delp->key || ret->key == delp->key) && delp != t->root)
      delp->right = ret;
    else
      delp->left = ret;

    return 0;
  }
  else if (del->left && key == del->left->key)
  {
    free(del->left);
    del->left = NULL;
    return 1;
  }
  else if (del->right && key == del->right->key)
  {
    free(del->right);
    del->right = NULL;
    return 1;
  }
  else
  {
    return 0;
  }
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n)
{
  // TODO: implement to_array
  return 0;
}
