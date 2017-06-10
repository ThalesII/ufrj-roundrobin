#ifndef VECTOR_H
#define VECTOR_H

/* Usage example:
 *     float *vec = NULL;
 *     float val = 10.0f;
 *     vec_append(&vec, &val);
 *     vec_remove(vec, 0);
 *     vec_free(vec);
 */

#define vec_length(vec) vec_length_(vec, sizeof *(vec))
#define vec_append(vecp, src) vec_append_(vecp, src, sizeof **(vecp))
#define vec_remove(vec, idx) vec_remove_(vec, idx, sizeof *(vec))

size_t vec_length_(void *vec, size_t size);

void vec_append_(void *vecp, void *src, size_t size);

void vec_remove_(void *vec, size_t idx, size_t size);

void vec_free(void *vec);

#endif
