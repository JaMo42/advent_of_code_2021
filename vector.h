/* Based on stb stretchy buffer
 * https://github.com/nothings/stb/blob/master/stb_ds.h
 */
#ifndef VECTOR_H
#define VECTOR_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct vector__header {
  size_t size;
  size_t capacity;
  char data[];
};

#ifndef VECTOR__DECLTYPE
# ifdef __cplusplus
#  define VECTOR__DECLTYPE(x) decltype(x)
# else /* __cplusplus */
#  ifdef __GNUC__
#   define VECTOR__DECLTYPE(x) typeof(x)
#  endif /* __GNUC__ */
# endif /* __cplusplus */
#endif /* VECTOR__DECLTYPE */

#ifndef VECTOR__MAYBE_UNSUED
# ifdef __cplusplus
#  define VECTOR__MAYBE_UNUSED [[maybe_unused]]
# else /* __cplusplus */
#  ifdef __GNUC__
#   define VECTOR__MAYBE_UNUSED __attribute__((unused))
#  else /* __GNUC__ */
#   define VECTOR__MAYBE_UNUSED
#  endif /* __GNUC__ */
# endif /* __cplusplus */
#endif /* VECTOR__MAYBE_UNUSED */

#ifndef VECTOR__HAS_STATEMENT_EXPRS
# ifdef __GNUC__
#  define VECTOR__HAS_STATEMENT_EXPRS
# endif /* __GNUC__ */
#endif /* VECTOR__HAS_STATEMENT_EXPRS */

/**
 * Parameters:
 *    v - vector
 *    e - element
 *    n - number of elements
 *    i - index of element
 *    T - type
 *  ... - initializer list elements
 */

#define vector__get(v) (((struct vector__header *)(v)) - 1)
#define vector__size(v) (vector__get(v)->size)
#define vector__capacity(v) (vector__get(v)->capacity)

/* Grow the vector so it can fit at least N more items. */
#define vector__grow(v, n) (*((void **)&(v)) = vector__grow_f((v), (n), sizeof(*(v))))
/* Check if the vector needs to grow to accommodate N more items. */
#define vector__needgrow(v, n) ((v) == NULL || vector__size(v) + (n) >= vector__capacity(v))
/* Ensure that the vector can fit N more items, grow it if necessary. */
#define vector__maybegrow(v, n) (vector__needgrow((v), (n)) ? vector__grow((v), (n)) : 0)



/* Gets the number of elements in the vector. */
#define vector_size(v)\
  ((v) == NULL ? 0 : vector__size(v))

/* Gets the number of elements that fit in the vector. */
#define vector_capacity(v)\
  ((v) == NULL ? 0 : vector__capacity(v))

/* Checks if the vector is empty. */
#define vector_empty(v)\
  ((v) == NULL ? 1 : vector__size(v) == 0)

/* Increases the capacity of the vector the fit at least N elements. */
#define vector_reserve(v, n)\
  ((n) > vector_capacity(v) ? vector_resize((v), (n)) : 0)

/* Reduces the vectors capacity to its size. */
#define vector_shrink_to_fit(v)\
  ((v) == NULL ? NULL : vector_resize((v), vector_size(v)))

/* Gets the last element of the vector. */
#define vector_back(v)\
  ((v)[vector__size(v) - 1])

/* Gets a pointer past the last element of the vector. */
#define vector_end(v)\
  ((v) == NULL ? NULL : ((v) + vector__size(v)))

/* Appends an element to the vector. */
#define vector_push(v, e)        \
  (vector__maybegrow((v), 1),    \
   (v)[vector__size(v)++] = (e))

#ifdef VECTOR__DECLTYPE
/* For vectors of structures or unions, appends a new element to the vector and
   constructs it in-place, variadic arguments are passed as initializer list
   for struct or union. */
#define vector_emplace_back(v, ...)                                 \
  (vector__maybegrow((v), 1),                                       \
   (v)[vector__size(v)++] = (VECTOR__DECLTYPE(*v)) { __VA_ARGS__ })
#endif

/* Gets and removes the last element of the vector. */
#define vector_pop(v)\
  ((v)[--vector__size(v)])

/* Inserts a new element into the vector at position I. */
#define vector_insert(v, i, e)                                \
  (((v) == NULL || (size_t)(i) >= vector_size(v))             \
   ? 0                                                        \
   : (vector__maybegrow((v), 1),                              \
      vector__shift((char*)(void*)(v), (i), 1, sizeof(*(v))), \
      (v)[(i)] = (e),                                         \
      ++vector__size(v)))

#ifdef VECTOR__DECLTYPE
/* Like vector_emplace_back but the new element is inserted at position I. */
#define vector_emplace(v, i, ...)                                 \
  (((v) == NULL || (size_t)(i) >= (vector__size (v)))             \
   ? 0                                                            \
   : (vector__maybegrow ((v), 1),                                 \
      vector__shift ((char *)(void *)(v), (i), 1, sizeof (*(v))), \
      (v)[(i)] = (VECTOR__DECLTYPE (*v)) { __VA_ARGS__ },         \
      ++vector__size (v)))
#endif

/* Removes the element at position I from the vector. */
#define vector_remove(v, i)                                        \
  (((v) == NULL || (size_t)(i) >= vector_size(v))                  \
   ? 0                                                             \
   : (vector__shift((char *)(void *)(v), (i+1), -1, sizeof(*(v))), \
      --vector__size(v)))

/* Removes N elements from the vector, starting at position I. */
#define vector_erase(v, i, n)                                               \
  (((v) == NULL || (size_t)(i) >= (vector__size (v) - (n)))                 \
   ? 0                                                                      \
   : (vector__shift ((char *)(void *)(v), (i)+(n), 0LL-(n), sizeof (*(v))), \
      vector__size (v) -= (n)))

/* Clears the contents of the vector. */
#define vector_clear(v)\
  ((v) == NULL ? 0 : (vector__size (v) = 0))

/* Resizes the vector. */
#define vector_resize(v, n)\
  (*((void **)&(v)) = vector__resize_f((v), (n), sizeof(*(v))))

/* Creates a new empty vector. */
#define vector_create(T, n)\
  ((T *)vector__create((n), sizeof(T)))

#ifdef VECTOR__HAS_STATEMENT_EXPRS
/* Creates a new vector with elements {X, ...} and the type of X as element
   type. */
#define vector_init(x, ...)                                                \
  ({                                                                       \
    VECTOR__DECLTYPE (x) __t[] = { x __VA_OPT__ (,) __VA_ARGS__ };         \
    size_t __l = sizeof (__t) / sizeof (x);                                \
    VECTOR__DECLTYPE (x) *__v = vector_create (VECTOR__DECLTYPE (x), __l); \
    memcpy (__v, __t, __l * sizeof (x));                                   \
    vector__size (__v) = __l;                                              \
    __v;                                                                   \
  })
#endif

/* Frees the vector. */
#define vector_free(v)\
  ((v) ? (free(vector__get(v)), 0) : 0)



static void *
vector__resize_f(void *data, size_t elems, size_t elem_size) {
  struct vector__header *v = (struct vector__header *)realloc (
    data ? vector__get (data) : NULL,
    elems * elem_size + sizeof (struct vector__header));
  if (v)
    {
      if (!data)
        v->size = 0;
      else if (elems < v->size)
        v->size = elems;
      v->capacity = elems;
      return v->data;
    }
  else
    {
      fputs("vector__resize_f: allocation failed\n", stderr);
      exit (1);
    }
}

VECTOR__MAYBE_UNUSED static void *
vector__grow_f(void *data, size_t size, size_t elem_size) {
  size_t min_needed = vector_size(data) + size;
  size_t default_growth = vector_capacity(data) << 1;
  size_t new_capacity = default_growth > min_needed ? default_growth : min_needed;
  return vector__resize_f(data, new_capacity, elem_size);
}

VECTOR__MAYBE_UNUSED static void
vector__shift(char *data, size_t index, long diff, size_t elem_size) {
  char *at = data + index * elem_size;
  size_t count = vector__size(data) - index;
  memmove(at + diff * elem_size, at, count * elem_size);
}

VECTOR__MAYBE_UNUSED static void *
vector__create(size_t capacity, size_t elem_size) {
  struct vector__header *v = (struct vector__header *)malloc(
    capacity * elem_size + sizeof(struct vector__header));
  v->size = 0;
  v->capacity = capacity;
  return (void *)v->data;
}

#endif /* !VECTOR_H */
