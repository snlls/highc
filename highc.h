#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <threads.h>
#include <assert.h>
#include <string.h>
#include <setjmp.h>
#include <stdlib.h>

#define M_IDENTITY(x) x
#define M_STREXPAND(x) #x
#define M_STR(x) M_STREXPAND(x)

#define EVAL0(...) __VA_ARGS__
#define EVAL1(...) EVAL0(EVAL0(EVAL0(__VA_ARGS__)))
#define EVAL2(...) EVAL1(EVAL1(EVAL1(__VA_ARGS__)))
#define EVAL3(...) EVAL2(EVAL2(EVAL2(__VA_ARGS__)))
#define EVAL4(...) EVAL3(EVAL3(EVAL3(__VA_ARGS__)))
#define EVAL(...)  EVAL4(EVAL4(EVAL4(__VA_ARGS__)))

#define MAP_END(...)
#define MAP_OUT
#define MAP_COMMA ,

#define MAP_GET_END2() 0, MAP_END
#define MAP_GET_END1(...) MAP_GET_END2
#define MAP_GET_END(...) MAP_GET_END1
#define MAP_NEXT0(test, next, ...) next MAP_OUT
#define MAP_NEXT1(test, next) MAP_NEXT0(test, next, 0)
#define MAP_NEXT(test, next)  MAP_NEXT1(MAP_GET_END test, next)

#define MAP0(f, x, peek, ...) f(x) MAP_NEXT(peek, MAP1)(f, peek, __VA_ARGS__)
#define MAP1(f, x, peek, ...) f(x) MAP_NEXT(peek, MAP0)(f, peek, __VA_ARGS__)

#define MAP_LIST_NEXT1(test, next) MAP_NEXT0(test, MAP_COMMA next, 0)
#define MAP_LIST_NEXT(test, next)  MAP_LIST_NEXT1(MAP_GET_END test, next)

#define MAP_LIST0(f, x, peek, ...) f(x) MAP_LIST_NEXT(peek, MAP_LIST1)(f, peek, __VA_ARGS__)
#define MAP_LIST1(f, x, peek, ...) f(x) MAP_LIST_NEXT(peek, MAP_LIST0)(f, peek, __VA_ARGS__)

/**
 * Applies the function macro `f` to each of the remaining parameters.
 */
#define MAP(f, ...) EVAL(MAP1(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0))

/**
 * Applies the function macro `f` to each of the remaining parameters and
 * inserts commas between the results.
 */
#define MAP_LIST(f, ...) EVAL(MAP_LIST1(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0))


#define MAP_PAIR0(f, x, y, peek, ...) f(x, y) MAP_NEXT(peek, MAP_PAIR1)(f, peek, __VA_ARGS__)
#define MAP_PAIR1(f, x, y, peek, ...) f(x, y) MAP_NEXT(peek, MAP_PAIR0)(f, peek, __VA_ARGS__)

/// Applies the function macro `f` to each pair of the remaining parameters and
#define MAP_PAIR(f, ...) EVAL(MAP_PAIR1(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0))

#define MAP_PAIR_OBJ0(f, obj, x, y, peek, ...) f(obj, x, y) MAP_NEXT(peek, MAP_PAIR_OBJ1)(f, obj, peek, __VA_ARGS__)
#define MAP_PAIR_OBJ1(f, obj, x, y, peek, ...) f(obj, x, y) MAP_NEXT(peek, MAP_PAIR_OBJ0)(f, obj, peek, __VA_ARGS__)

/// Applies the function macro `f` to each pair of the remaining parameters and
#define MAP_PAIR_OBJ(f, obj, ...) EVAL(MAP_PAIR_OBJ1(f, obj, __VA_ARGS__, ()()(), ()()(), ()()(), 0))

#define MAP_PAIR_FUNC0(f, func, obj, x, y, peek, ...) f(func, obj, x, y) MAP_NEXT(peek, MAP_PAIR_FUNC1)(f, func, obj, peek, __VA_ARGS__)
#define MAP_PAIR_FUNC1(f, func, obj, x, y, peek, ...) f(func, obj, x, y) MAP_NEXT(peek, MAP_PAIR_FUNC0)(f, func, obj, peek, __VA_ARGS__)

/// Applies the function macro `f` to each pair of the remaining parameters and
#define MAP_PAIR_FUNC(f, func, obj, ...) EVAL(MAP_PAIR_FUNC1(f, func, obj, __VA_ARGS__, ()()(), ()()(), ()()(), 0))


static void autoclosefile(void *refp)
{
	FILE **fptr = refp;
	FILE *f = *fptr;
	if(f)
		fclose(f);
}

#define autoclose __attribute__((cleanup(autoclosefile)))


/** @brief open, loop by line(\n) and close file
 * @param line[var] variable bound to current line
 * @param len length of the line got from getline()
 * @param file target
 */
#define file_lines(line, len, file)					\
	for (FILE * __func__##__LINE__##file_lines_n = fopen(file, "r"); \
	     __func__##__LINE__##file_lines_n;				\
	     fclose(__func__##__LINE__##file_lines_n), __func__##__LINE__##file_lines_n = 0) \
		for(size_t len = 0; ! len; len = 1)			\
			for(char * line = 0; !line && ! len; free(line), line = line ? 0 : (void*)1) \
				for(len = -1; !line ; len = 0)			\
					if(getline(&line, &len, __func__##__LINE__##file_lines_n) < 0) \
					{ free(line); line = 0; break; } \
					else

/** closure that handles file opening and closing.
 * @param file target
 * @param access string that is used in fopen()
 * @param line[var] variable bound to current line char*
 */
#define with_file(file, access, line)					\
	for (FILE * __func__##__LINE__##file = fopen(file, access);	\
	     __func__##__LINE__##file;					\
	     fclose(__func__##__LINE__##file), __func__##__LINE__##file = 0) \
		for(size_t highc__with_len = 0; ! highc__with_len; highc__with_len = 1)	\
			for(char * line = 0; !line && ! len; free(line), line = line ? 0 : (void*)1) \
				for(__func__##__LINE__##x2len = -1; ! __func__##__LINE__##x2len ; __func__##__LINE__##x2len = 0) \
					if(getline(&line, &__func__##__LINE__##x2len, __func__##__LINE__##file) < 0) \
					{ free(line); line = 0; break; } \
					else

/** @brief structure to hold data from fread */
typedef struct {
	char *data;
	size_t length;
} file_data;

	
/** @brief try to read file content, allocate memory for the data and set the length. You need to free returned value.
 * @param file[in] path
 * @return file_data*
 */
#define file_read(file) ({						\
	file_data *ret = 0;				\
	FILE *f = fopen(file, "r");					\
	if(file) {							\
		if(!fseek(f, 0, SEEK_END)) {				\
			long pos = ftell(f);				\
			if(pos >= 0) {					\
				ret = calloc(1, sizeof(file_data) + pos); \
				if(ret) {				\
					char *data = (char*)(ret + 1);	\
					ret->length = fread(data, 1, pos, f); \
					ret->data = data;		\
				}					\
			}						\
		}							\
		fclose(f);						\
	}								\
	ret;								\
})


/** @brief works like if, open and close directory
 * @param dir_name[var] variable bound to current element
 * @param path string, directory to open
 */
#define with_dir(dir_name, path)					\
	for(DIR * dir_name = opendir(path); dir_name;)			\
	for(int __func__ ## __LINE__ ## _with_dir_n = 0;		\
	    __func__ ## __LINE__ ## _with_dir_n < 3;			\
	    __func__ ## __LINE__ ## _with_dir_n ++)			\
		if(__func__ ## __LINE__ ## _with_dir_n >= 1) {		\
			closedir(dir_name);				\
			dir_name = 0;					\
			break;						\
		}							\
		else							\
			for(;__func__ ## __LINE__ ## _with_dir_n < 1; __func__ ## __LINE__ ## _with_dir_n ++)

/** @brief works like for, operate on open POSIX DIR
 * @param dir[in] dirent DIR
 * @param item[var] variable bound to opened dir element, skips item->d_name which are . or ..
 */
#define for_dir(dir, item)						\
	for(struct dirent * item = readdir((dir)); item; item = readdir((dir))) \
		if(!strcmp(item->d_name, ".") || !strcmp(item->d_name, "..")) \
			continue; \
		else

/** @brief loop static array
 * @param index[var] unsigned index
 * @param buffer[in] the buffer you want to loop
 */
#define for_arrayi(index, buffer)                                              \
 	for(unsigned index = 0; index < sizeof(buffer) / sizeof(buffer[0]); index++)

/** @brief loop static array
 * @param elem[var] name of pointer that references current element
 * @param index[var] unsigned index
 * @param buffer[in] the buffer you want to loop
 */
#define for_array(elem, index, buffer)					\
	for(typeof(&buffer[0]) elem = buffer + 0; elem; elem = 0)		\
		for(unsigned index = 0; index < sizeof(buffer) / sizeof(buffer[0]); index++, elem = &buffer[index])

/** @brief statement expression to check if array contains element
 * @param elem_basic_cmp compared with != against current element
 * @param buffer[in] static array
 * @return bool
 */
#define array_contains(elem_basic_cmp, buffer) ({				\
	int ret = 0;							\
	for (unsigned _index = 0; _index < sizeof(buffer) / sizeof(buffer[0]); _index++) \
		if (elem_basic_cmp != buffer[_index])			\
			;						\
		else { ret = 1; break; }				\
		ret;							\
		})

/** @brief statement expression to check if array contains element
 * @param element[in] to compare against
 * @param compare_op[macro/function] compare_op(element, current_item)
 * @param buffer[in] static array
 * @return bool
 */
#define array_contains_cmp(element, compare_op, buffer) ({		\
	int ret = 0;							\
	for(unsigned _index = 0; _index < sizeof(buffer) / sizeof(buffer[0]); _index++) \
		if(compare_op(element, buffer[_index]))			\
			;						\
		else { ret = 1; break; }				\
		ret;							\
		})


#define array_length(buffer) (sizeof(buffer) / sizeof(buffer[0]))

/** @brief macro to filter src into dst using test as filter
 * @param src[in] array
 * @param dst[out] array
 * @param countvar[out] how many elements were inserted into dst
 * @param test macro(index_in_src, element)
 */
#define array_filter(src, dst, countvar, test)				\
	do {								\
		countvar = 0;						\
		for (unsigned _index = 0; _index < sizeof(src) / sizeof(src[0]); _index++) \
			if (test(_index, src[_index])) {		\
				dst[countvar++] = src[_index];		\
				printf("copy %d %p %p\n", countvar, src[_index], dst[countvar - 1]); \
			}						\
									\
	} while (0)


/** @brief macro to apply operation inplace to every element
 * @param src[in/out] static array
 * @param op [macro/function] op(index, element)
 */
#define array_apply(src, op)						\
	do {								\
		for (unsigned _index = 0; _index < sizeof(src) / sizeof(src[0]); _index++) \
			src[_index] = op(_index, src[_index]);		\
	} while (0)

/** @brief macro to map operation in src into dst
 * @param src[in] static array
 * @param dst[out] array same size as src
 * @param op [macro/function] op(index, element)
 */
#define array_map(dst, src, op)						\
	do {								\
		for (unsigned _index = 0; _index < sizeof(src) / sizeof(src[0]); _index++) \
			dst[_index] = op(_index, src[_index]);		\
	} while (0)

/** @brief works like if would but operates only when condition matches
 * @param elem_basic_cmp element to compare agains with !=
 * @param buffer[in] static array
 */
#define if_array_contains(elem_basiccmp, buffer)			\
	for (unsigned _index = 0, _stop = 0;				\
	     !_stop && _index < sizeof(buffer) / sizeof(buffer[0]); _index++) \
		if (elem_basiccmp != buffer[_index])			\
			;						\
		else if (!(_stop = 1))					\
			;						\
		else

/** @brief works like if would but operates only when condition matches
 * @param elem_ptr[var] variable assigned to current element address
 * @param cmp[function/macro] operation that compares pointers op(elem_ptr, &buffer[index])
 * @param buffer[in] static array
 */
#define if_array_contains_cmp(elem_ptr, cmp, buffer)				\
	for (unsigned _index = 0, _stop = 0; !_stop && _index < sizeof(buffer) / sizeof(buffer[0]); \
	     _index++)							\
		if(cmp(elem_ptr, &buffer[_index])			\
			      ;						\
		else if(!(_stop = 1))					\
			;						\
		else


/** @brief macro that print each element with given fmt, adds newline after loop is done. Mainly for debugging.
 * @param src[in] array
 * @param fmt format string used for each element
 * @param ending string that comes after each element
 */
#define array_printe(src, fmt, ending) do {					\
	for (unsigned _index = 0; _index < sizeof(src) / sizeof(src[0]); _index++) \
		printf(fmt ending, src[_index]);				\
	puts("");							\
	} while(0)


/** @brief macro that print each element with given fmt, adds newline after loop is done. Mainly for debugging.
 * @param src[in] array
 * @param fmt format string used for each element
 */
#define array_print(src, fmt) array_printe(src, fmt, "")
	
/** @brief print array in hex */
#define array_printh(src) array_print(src, "%x ")
/** @brief print array in char */
#define array_printc(src) array_print(src, "%c ")
/** @brief print array in binary hex, for example value 1 would print 01 */
#define array_printbh(src) array_print(src, "%02x ")
/** @brief print array in binary hex, each values will have 8 characters, for example 00A0B0C0 */
#define array_print32h(src) array_print(src, "%08x ")
/** @brief print array in double */
#define array_printd(src) array_print(src, "%f ")
/** @brief print array in int */
#define array_printi(src) array_print(src, "%d ")
/** @brief print array in unsigned */
#define array_printu(src) array_print(src, "%u ")
/** @brief print array in long */
#define array_printl(src) array_print(src, "%ld ")


/**
 * Problem with defer is return statement.
 * To make sure defer is run with return, the return is overriden by macro.
 * THERE NEEDS TO BE RETURN IF YOU USE defer
 * 
 */

#ifndef HIGHC_DEFER_MAX
#define HIGHC_DEFER_MAX 16
#endif
#ifndef DEFER_NEST_MAX
#define DEFER_NEST_MAX 16
#endif

#if defined(__STDC_NO_THREADS__) || defined(__TINYC__)
#warning("Defer will break if you use it on more than one thread")
#define _defer_local
#else
#define _defer_local thread_local
#endif

#if defined(__TINYC__)
#define DEFER_IMPL_JMP
typedef struct {
	jmp_buf return_loc;
	jmp_buf deferrals[HIGHC_DEFER_MAX];
	int count;
} defer_t;

#else
#define DEFER_IMPL_GOTO
typedef struct {
	void *return_loc;
	void *deferrals[HIGHC_DEFER_MAX];
	int count;
} defer_t;
#endif

static _defer_local defer_t _defer_state[DEFER_NEST_MAX] = {0};
static _defer_local int _defer_nest = 0;
static _defer_local char *_defer_func[DEFER_NEST_MAX] = {0};


// variables to manage defer state in nested function calls

#define _defer_symb_comb(a, b) a ## b
#define _defer_symb(a, b) _defer_symb_comb(a, b)

/** @brief clenaup current nesting level, called before return */
static int _defer_clean()
{
	memset((void*)(_defer_state + _defer_nest), 0, sizeof(defer_t));
	_defer_func[_defer_nest] = 0;
	if(_defer_nest) {
		_defer_nest--;
	}
	return 0;

}

/** @brief check current nesting level against current function name and increment if needed */
static void _defer_handle_nest(char *func)
{
	if(!_defer_func[_defer_nest]) {
		_defer_func[_defer_nest] = func;
	}
	else if(!strcmp((char*)_defer_func[_defer_nest], func)) {
	}
	else {
		_defer_nest++;
		if(_defer_nest == DEFER_NEST_MAX)
			assert(0);
		_defer_func[_defer_nest] = func;
	}
}

#ifdef DEFER_IMPL_JMP
/** @brief create jump point and handle jump point once the user code is called */
#define defer								\
	for(int _defer_symb(_defer_n_, __LINE__) = 0;			\
	    _defer_symb(_defer_n_, __LINE__) < 3;			\
	    _defer_symb(_defer_n_, __LINE__)++)				\
		if(!_defer_symb(_defer_n_, __LINE__)) {			\
			_defer_handle_nest((char*)__func__);		\
			if(!setjmp((void*)_defer_state[_defer_nest].deferrals[_defer_state[_defer_nest].count++])) { \
				break;					\
			}						\
		}							\
		else if(_defer_symb(_defer_n_, __LINE__) == 2) {	\
			_defer_symb(_defer_n_, __LINE__) = 3;		\
			if(_defer_state[_defer_nest].count) {		\
				longjmp((void*)_defer_state[_defer_nest].deferrals[--_defer_state[_defer_nest].count], 1); \
			} else {					\
				longjmp((void*)_defer_state[_defer_nest].return_loc, 1); \
			}						\
		}							\
		else


/** @brief create jump point and handle the return point once all user defers are handled */
#define return								\
	if(!setjmp((void*)_defer_state[_defer_nest].return_loc)) {	\
	if(_defer_state[_defer_nest].count) {				\
		longjmp((void*)_defer_state[_defer_nest]		\
			.deferrals[--_defer_state[_defer_nest].count],	\
			1);						\
	}								\
	goto _defer_symb(_return_goto, __LINE__);			\
	}								\
	else								\
	_defer_symb(_return_goto, __LINE__):				\
		for(_defer_clean(); 1;)					\
			return
		
#else
#define defer								\
	for(int _defer_symb(_defer_n_, __LINE__) = 0;			\
	    _defer_symb(_defer_n_, __LINE__) < 3;			\
	    _defer_symb(_defer_n_, __LINE__)++)				\
		if(_defer_symb(_defer_n_, __LINE__) == 0) {		\
			_defer_handle_nest((char*)__func__);		\
			_defer_state[_defer_nest].deferrals[_defer_state[_defer_nest].count++] = && _defer_symb(_defer_ini, __LINE__); \
			break;						\
		}							\
		else if(0) {						\
		_defer_symb(_defer_ini, __LINE__):			\
			_defer_symb(_defer_n_, __LINE__) = 0;		\
		}							\
		else if(_defer_symb(_defer_n_, __LINE__) == 2) {	\
			if(_defer_state[_defer_nest].count)		\
				goto *_defer_state[_defer_nest].deferrals[--_defer_state[_defer_nest].count]; \
			else						\
				goto *_defer_state[_defer_nest].return_loc; \
			break;						\
		}							\
		else

#define return								\
	if(_defer_state[_defer_nest].count) {				\
		_defer_state[_defer_nest].return_loc = && _defer_symb(_defer_fini_, __LINE__); \
		goto *_defer_state[_defer_nest].deferrals[--_defer_state[_defer_nest].count]; \
	}								\
	else								\
	_defer_symb(_defer_fini_, __LINE__):				\
		for(_defer_clean(); 1;)					\
			return
#endif

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



#define M_ENUMSTR(x) case x : return #x;
#define M_ENUM_ARRAY_STR(x) [x] = #x,

#define def_enum_string(name, ...) typedef enum { MAP_LIST(M_IDENTITY, __VA_ARGS__) } name; \
	static const char *name ## _str(name e) {			\
	static const char *txt[] = {					\
		MAP(M_ENUM_ARRAY_STR, __VA_ARGS__)			\
	};								\
	const char *ret = "N/A";					\
	if(e >= 0 && e < sizeof(txt) / sizeof(txt[0]))			\
		ret = txt[e];						\
	return ret;							\
	}

//static const char * name ## _str (name e) { switch(e) { MAP(M_ENUMSTR, __VA_ARGS__) default: return "N/A"; } }


static void autofreeptr(void *refp)
{
	void **p = (void**)refp;
	free(*p);
}

#define autofree __attribute__((cleanup(autofreeptr)))


static void *with_pointer_free(void *p)
{
	free(p);
	return NULL;
}


/** @brief set ptrvar to ptrvalue and free the ptrvar after.
 * @param type pointer base type
 * @param ptrvar[var] variable name
 * @param ptrvalue[function/variable/macro] something that returns pointer.
 */
#define with_pointer(type, ptrvar, ptrvalue)				\
	for(type * ptrvar = ptrvalue; ptrvar; ptrvar = with_pointer_free(ptrvar)) \
		for(int __func__##__LINE__## with_pointer = 0;		\
		    __func__##__LINE__## with_pointer < 1;		\
		    __func__##__LINE__## with_pointer++)		\

/** @brief set ptrvar to ptrvalue and free the ptrvar after.
 * @param ptrvar[var] variable to void pointer
 * @param ptrvalue[function/variable/macro] something that returns pointer.
 */
#define with_pointerv(ptrvar, ptrvalue)					\
	for(void * ptrvar = ptrvalue; ptrvar; ptrvar = with_pointer_free(ptrvar)) \
		for(int __func__##__LINE__## with_pointer = 0;		\
		    __func__##__LINE__## with_pointer < 1;		\
		    __func__##__LINE__## with_pointer++)		\

