#pragma once

/** @brief loop vector
 * @param index[var] unsigned index
 * @param buffer[in] the buffer you want to loop
 * @param len[in] vector length
 */
#define for_vectori(index, buffer, len)			\
 	for(unsigned index = 0; index < (len); index++)

/** @brief loop vector
 * @param elem[var] name of pointer that references current element
 * @param index[var] unsigned index
 * @param buffer[in] the buffer you want to loop
 * @param len[in] vector length
 */
#define for_vector(elem, index, buffer, len)				\
	for(typeof(&buffer[0]) elem = buffer + 0; elem; elem = 0)		\
		for(unsigned index = 0; index < (len); index++, elem = &buffer[index])

/** @brief statement expression to check if vector contains element
 * @param elem_basic_cmp compared with != against current element
 * @param buffer[in] vector
 * @param len[in] vector length
 * @return bool
 */
#define vector_contains(elem_basic_cmp, buffer, len) ({			\
	int ret = 0;							\
	for (unsigned _index = 0; _index < (len); _index++)		\
		if (elem_basic_cmp != buffer[_index])			\
			;						\
		else { ret = 1; break; }				\
		ret;							\
		})

/** @brief statement expression to check if vector contains element
 * @param element[in] to compare against
 * @param compare_op[macro/function] compare_op(element, current_item)
 * @param buffer[in] vector
 * @param len[in] vector length
 * @return bool
 */
#define vector_contains_cmp(element, compare_op, buffer, len) ({	\
	int ret = 0;							\
	for(unsigned _index = 0; _index < (len); _index++)		\
		if(compare_op(element, buffer[_index]))			\
			;						\
		else { ret = 1; break; }				\
		ret;							\
		})


/** @brief macro to filter src into dst using test as filter
 * @param src[in] vector
 * @param len[in] vector length
 * @param dst[out] vector
 * @param countvar[out] how many elements were inserted into dst 
 * @param test macro(index_in_src, element)
 */
#define vector_filter(src, dst, len, countvar, test)			\
	do {								\
		countvar = 0;						\
		for (unsigned _index = 0; _index < (len); _index++)	\
			if (test(_index, src[_index])) {		\
				dst[countvar++] = src[_index];		\
				printf("copy %d %p %p\n", countvar, src[_index], dst[countvar - 1]); \
			}						\
									\
	} while (0)


/** @brief macro to apply operation inplace to every element
 * @param src[in/out] vector
 * @param len[in] vector length
 * @param op [macro/function] op(index, element)
 */
#define vector_apply(src, len, op)					\
	do {								\
		for (unsigned _index = 0; _index < (len); _index++)	\
			src[_index] = op(_index, src[_index]);		\
	} while (0)

/** @brief macro to map operation in src into dst
 * @param src[in] vector
 * @param dst[out] vector same size as src
 * @param len[in] vector length
 * @param op [macro/function] op(index, element)
 */
#define vector_map(dst, src, len, op)					\
	do {								\
		for (unsigned _index = 0; _index < (len); _index++)	\
			dst[_index] = op(_index, src[_index]);		\
	} while (0)

/** @brief works like if would but operates only when condition matches
 * @param elem_basic_cmp element to compare agains with !=
 * @param buffer[in] vector
 * @param len[in] vector length
 */
#define if_vector_contains(elem_basiccmp, buffer, len)			\
	for (unsigned _index = 0, _stop = 0;				\
	     !_stop && _index < (len); _index++)			\
		if (elem_basiccmp != buffer[_index])			\
			;						\
		else if (!(_stop = 1))					\
			;						\
		else

/** @brief works like if would but operates only when condition matches
 * @param elem_ptr[var] variable assigned to current element address
 * @param cmp[function/macro] operation that compares pointers op(elem_ptr, &buffer[index])
 * @param buffer[in] vector
 * @param len[in] vector length
 */
#define if_vector_contains_cmp(elem_ptr, cmp, buffer, len)		\
	for (unsigned _index = 0, _stop = 0; !_stop && _index < (len);	\
	     _index++)							\
		if(cmp(elem_ptr, &buffer[_index])			\
			      ;						\
		else if(!(_stop = 1))					\
			;						\
		else


/** @brief macro that print each element with given fmt, adds newline after loop is done. Mainly for debugging.
 * @param src[in] vector
 * @param len vector length
 * @param fmt format string used for each element, needs to be compile time string
 * @param prefix prefix to add to each element
 * @param newline_n if positive add newline after n elements
 */
#define vector_print(src, len, fmt, prefix, newline_n) do {		\
		for (unsigned _index = 0, newline_counter = 0; _index < (len); _index++) { \
			printf(prefix fmt, src[_index]);		\
			if(newline_n && ++newline_counter >= newline_n) { \
				newline_counter = 0;			\
				printf("\n");				\
			}						\
		}							\
		puts("");						\
	} while(0)

