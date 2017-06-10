#ifndef VECTOR_H
#define VECTOR_H

#define length(vec) length_(vec, sizeof *(vec))
#define append(vecp, src) append_(vecp, src, sizeof **(vecp))
#define remove_at(vec, idx) remove_at_(vec, idx, sizeof *(vec))

size_t length_(void *vec, size_t size);

void append_(void *vecp, void *src, size_t size);

void remove_at_(void *vec, size_t idx, size_t size);

#endif
