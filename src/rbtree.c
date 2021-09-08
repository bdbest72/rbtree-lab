#include "rbtree.h"
#include <malloc.h>

//test case assert 통과를 위해
node_t *head_node;
int node_count;

void *head_node_to_t_root(rbtree *t)
{
  t->root = head_node->left;
}

static int comp(const void *p1, const void *p2)
{
  const key_t *e1 = (const key_t *)p1;
  const key_t *e2 = (const key_t *)p2;
  if (*e1 < *e2)
  {
    return -1;
  }
  else if (*e1 > *e2)
  {
    return 1;
  }
  else
  {
    return 0;
  }
};
// int compare(const void *a, const void *b)
// {
//   return (*(int *)a - *(int *)b);
// }
//여기까지

rbtree *new_rbtree(void)
{
  rbtree *p = (rbtree *)calloc(sizeof(rbtree), 1);
  return p;
}

void delete_rbtree(rbtree *t)
{
  // TODO: reclaim the tree nodes's memory
  //현재 head_node->left를 사용할경우 segmentation error occurs
  remove_subtree(t->root);
  // head_node->left = NULL;
  // t->root = NULL;
  if (head_node)
  {
    // printf("/---대가리 컷!---/");
    free(head_node);
  }

  node_count = 0;
  free(t);
}

void remove_subtree(node_t *node)
{
  if (node != NULL)
  {
    remove_subtree(node->left);
    remove_subtree(node->right);
    free(node);
    // return;
  }
  // return;
}

node_t *_Rotate(const key_t key, node_t *pivot, rbtree *t)
{
  node_t *child, *gchild;

  if ((key > pivot->key || key == pivot->key) && pivot != head_node)
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
  if ((key > pivot->key || key == pivot->key) && pivot != head_node)
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
    node_t *temp;
    temp = malloc(sizeof(node_t));
    t->root = temp;
    t->root->key = key;
    t->root->color = RBTREE_BLACK;
    t->root->left = t->root->right = t->root->parent = NULL;
    node_count = 1;

    //headnode 생성
    head_node = malloc(sizeof(node_t));
    head_node->left = t->root;
    head_node->right = head_node->parent = NULL;
    head_node->color = RBTREE_BLACK;
    // t->root->parent = head_node;

    return t->root;
  }

  //두번째+ 삽입
  node_t *n, *p, *gp, *ggp;
  //수정 01:
  ggp = gp = p = (node_t *)head_node;
  n = head_node->left;

  while (n)
  {
    if (key == n->key)
    {
      head_node_to_t_root(t);
      return 0;
    }
    //중복 방지

    if (n->left && n->right && n->left->color == RBTREE_RED && n->right->color == RBTREE_RED)
    {
      //하향 색변환 가능한 상황 자식 노드 두개가 빨강이고 나는 검정일때
      n->color = RBTREE_RED;
      n->left->color = n->right->color = RBTREE_BLACK;

      // 빨강이 연속되기에 회전 필요
      if (p->color == RBTREE_RED)
      {
        gp->color = RBTREE_RED;
        if ((key > gp->key) != (key > p->key))
          p = _Rotate(key, gp, t);
        n = _Rotate(key, ggp, t);
        n->color = RBTREE_BLACK;
      }

      // 루트는 언제나 검정
      head_node->left->color = RBTREE_BLACK;
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
  node_count++;

  if (key > p->key && p != head_node)
    p->right = temp;
  else
    p->left = temp;

  //부모가 빨강이면 한번더 회전 / 삽입 노드는 빨강이기 때문;
  if (p->color == RBTREE_RED)
  {
    gp->color = RBTREE_RED;
    if ((key > gp->key) != (key > p->key))
      p = _Rotate(key, gp, t);
    temp = _Rotate(key, ggp, t);
    temp->color = RBTREE_BLACK;
  }

  //뿌리는 검정으로
  head_node->left->color = RBTREE_BLACK;

  head_node_to_t_root(t);
  return head_node->left;
}

node_t *rbtree_find(const rbtree *t, const key_t key)
{
  // TODO: implement find
  node_t *s;

  s = head_node->left;
  while (s && !(key == s->key))
  {
    if (key > s->key)
      s = s->right;
    else
      s = s->left;
  }
  if (s == 0)
    return 0;
  // printf("찾았다! %d\n", s->key);
  return s;
}

node_t *rbtree_min(const rbtree *t)
{
  // TODO: implement find
  node_t *s;

  s = head_node->left;
  while (s && s->left)
  {
    s = s->left;
  }

  // printf("최소 값 찾았다! : %d\n", s->key);

  return s;
}

node_t *rbtree_max(const rbtree *t)
{
  // TODO: implement find
  node_t *s;

  s = head_node->left;
  while (s && s->right)
  {
    s = s->right;
  }

  // printf("최대 값 찾았다! : %d\n", s->key);

  return s;
}

int rbtree_erase(rbtree *t, node_t *p)
{
  // TODO: implement erase
  node_t *delgp, *delp, *del, *sib;

  int value = p->key;

  //루트 노드부터 탐색 시작
  delgp = delp = (node_t *)head_node;
  del = head_node->left;
  sib = 0;

  //leaf노드까지 탐색
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

    if (del != head_node->left && _Is2Node(del))
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
  if (del != head_node->left && _Is2Node(del))
  {
    if (!_BorrowKey(delgp, delp, del, sib, t))
      _BindNode(delp);
  }
  if (_DelLeafNode(value, delp, del, t))
  {
    node_count--;
    head_node_to_t_root(t);
    return 1;
  }
  else
  {
    head_node_to_t_root(t);
    return 0;
  }
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
  if ((p->left == NULL && p->right == NULL) || (p->left == RBTREE_BLACK && p->right == RBTREE_BLACK))
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

  if (head_node->left->color == RBTREE_RED)
    head_node->left->color = RBTREE_BLACK;
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
  //첫번쨰 if문
  if (key == del->key && !del->left && !del->right)
  {
    // red leaf 나 black leaf 인 경우
    free(del);
    if ((key > delp->key || key == delp->key) && delp != head_node)
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
    else if (del->right)
    {
      del->right->left = del->left;
      ret = del->right;
      ret->color = RBTREE_BLACK;
      free(del);
    }
    if ((ret->key > delp->key || ret->key == delp->key) && delp != head_node)
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
  int i = 0;
  add_to_array(head_node->left, arr, i);

  //확인용
  // for (int i = 0; i < n; i++)
  // {
  //   printf(" %d ", arr[i]);
  //   // assert(arr[i] == res[i]);
  // }
  printf("끝! \n\n");

  qsort((void *)arr, n, sizeof(key_t), comp);
  return 0;
}

int add_to_array(node_t *node, key_t *arr, int i)
{
  if (node == NULL)
    return i;

  printf(" %d -", node->key);
  arr[i] = node->key;
  i++;

  if (node->left != NULL)
    i = add_to_array(node->left, arr, i);
  if (node->right != NULL)
    i = add_to_array(node->right, arr, i);

  return i;
}

void rbtree_print(rbtree *t, node_t *node)
{
  printf(" ( %d ", node->key);
  if (node->color == RBTREE_BLACK)
    printf(" B ) -");
  else
    printf(" R ) -");

  if (node->left != NULL)
    rbtree_print(t, node->left);
  if (node->right != NULL)
    rbtree_print(t, node->right);
}

//해결
//head node의 left 값만 변경되고, t->root의 값을 바꿔주는 로직이 없엇기에
// 마지막 return 전에 최종 headnode값으,로 t-root의 값을 변경 시켜주는 함수
// 그리고 t->root로 잡혀있던 기존 기준들 head-left로 변경