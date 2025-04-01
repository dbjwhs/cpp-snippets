#! /usr/bin/env python3

"""
Pagination
Return a string representation of a pagination navigation bar
given the current page number, the total number of pages and
the maximum number of pages to return in the string.
Always return the first and last page and return the current
page in brackets (e.g. '[7]').
Ellipses ('...') will be placed between non-consecutive page
numbers to indicate omitted pages and the current page will
be centered as much as possible in returned pages.

Example input and output:

paginate(1, 11, 11)	== "[1] 2 3 4 5 6 7 8 9 10 11"
paginate(1, 30, 11)	== "[1] 2 3 4 5 6 7 8 9 10 ... 30"
paginate(6, 10, 11)	== '1 2 3 4 5 [6] 7 8 9 10'
paginate(6, 30, 11)	== "1 2 3 4 5 [6] 7 8 9 10 ... 30"
paginate(7, 30, 11)	== "1 ... 3 4 5 6 [7] 8 9 10 11 ... 30"
paginate(24, 30, 11) == "1 ... 20 21 22 23 [24] 25 26 27 28 ... 30"
paginate(27, 30, 11) == "1 ... 21 22 23 24 25 26 [27] 28 29 30"
paginate(30, 30, 11) == "1 ... 21 22 23 24 25 26 27 28 29 [30]"
paginate(1, 30, 10)	== "[1] 2 3 4 5 6 7 8 9 ... 30"
paginate(30, 30, 10) == "1 ... 22 23 24 25 26 27 28 29 [30]"

Assumptions:

- Add odd number to left side of current
  e.g. '1 ... 7 8 9 [10] 11 12 13 14 ... 200' (for 10,200,10).

- In case of total being less than 3, return first
(and last if total == 2) with appropriate brackets

- If total is 0 and curPage is 0, return ''

- If max < 3, treat max as 3 (return '1 ... c ... total')

"""

def page(page: int, curPage: int) -> str:
	"""Returns page number as string or '[page]' if page is curPage."""
	return f"[{page}]" if curPage == page else f"{page}"

def paginate(curPage: int, totalPages: int, maxVisiblePages: int) -> str:
	"""
	Return a string of paginated page numbers for a navigation bar.

	Args:
		curPage (int): Current page number
		totalPages (int): Total number of pages
		maxVisiblePages (int): Maximum number of pages to show

	Returns:
		str: A string of paginated page numbers with ellipses if needed with
			 the current page number in brackets (e.g. '[7]'). To the extent
			 possible, an equal number of pages will be shown to the left and
			 right of the current page but the first and last page are always
			 present. Skipped pages are indicated with an ellipsis ('...').

	Examples:
	>>> paginate(1, 11, 11)
	'[1] 2 3 4 5 6 7 8 9 10 11'
	>>> paginate(1, 30, 10)
	'[1] 2 3 4 5 6 7 8 9 ... 30'
	>>> paginate(7, 30, 10)
	'1 ... 4 5 6 [7] 8 9 10 11 ... 30'
	>> paginate(30, 30, 10)
	'1 ... 22 23 24 25 26 27 28 29 [30]'

	"""
	# handle invalid maxVisiblePages
	maxP = 3 if maxVisiblePages < 3 else maxVisiblePages
	lastP = totalPages
	if lastP <= 3 or maxP >= lastP: # no ellipses
		pages = list(range(1, lastP + 1))
	else:
		# secondPage is the first page printed (after 1)
		# penultimatePage is the next to last page printed
		# e.g.
		#   1 [...] secondPage..penultimatePage [...] last
		mid = maxP // 2 # mid rounds down for odd numbers
		extra = maxP % 2 # added to left side
		secondPage = max(0, curPage - mid - extra) + 2 # starts on 2nd page
		penultimatePage = secondPage + maxP - 2 # 2 for the start and end
		if penultimatePage > lastP:
			# overflowed last page, adjust both backwards
			overflow = penultimatePage - lastP
			penultimatePage -= overflow
			secondPage -= overflow

		# Form pages list
		pages = [1]
		if secondPage > 2:
			pages.append('...')
		pages.extend(range(secondPage, penultimatePage))
		if penultimatePage < lastP:
			pages.append('...')
		pages.append(lastP)

	return ' '.join([page(p, curPage) for p in pages])


# Test cases
if __name__ == '__main__':
	testCases = [
		# from specification
		[ 1, 11, 11, "[1] 2 3 4 5 6 7 8 9 10 11"],		# 1st page, all shown
		[ 1, 11, 10, "[1] 2 3 4 5 6 7 8 9 ... 11"],		# 1st page, one skipped
		[ 1, 30, 11, "[1] 2 3 4 5 6 7 8 9 10 ... 30"],	# 1st page, more # skipped
		[ 6, 10, 11, "1 2 3 4 5 [6] 7 8 9 10"],			# middle page, all shown
		[ 6, 30, 11, "1 2 3 4 5 [6] 7 8 9 10 ... 30"],	# middle page, last skipped
		[ 7, 30, 11, "1 ... 3 4 5 6 [7] 8 9 10 11 ... 30"], # even pages around curP
		[24, 30, 11, "1 ... 20 21 22 23 [24] 25 26 27 28 ... 30"], # larger curPage
		[27, 30, 11, "1 ... 21 22 23 24 25 26 [27] 28 29 30"], # No ellipse on end
		[30, 30, 11, "1 ... 21 22 23 24 25 26 27 28 29 [30]"], # last page
		[ 1, 30, 10, "[1] 2 3 4 5 6 7 8 9 ... 30"],		# 1st page, ellipse on end
		[30, 30, 10, "1 ... 22 23 24 25 26 27 28 29 [30]"], # as above, curP is last

		# extra test cases
		[ 2, 30, 11, "1 [2] 3 4 5 6 7 8 9 10 ... 30"],	# 1st page, more # skipped
		[ 0,  1, 20, "1"],		# invalid input, one page but current is invalid (0)
		[ 1,  1, 10, "[1]"], 	# only one page, current is first
		[ 1,  2, 10, "[1] 2"],	# only two pages, current is first
		[ 2,  2, 10, "1 [2]"],	# only two pages, current is last
		[ 10, 200, 11, "1 ... 6 7 8 9 [10] 11 12 13 14 ... 200"], # centered
		[ 10, 200, 10, "1 ... 7 8 9 [10] 11 12 13 14 ... 200"],	  # offset curPage
		[ 1,  1, 5, "1"],		# invalid input, one page but current is invalid (0)
	]

	for c in testCases:
		pag = paginate(c[0], c[1], c[2])
		if (pag != c[3]):
			print(f"FAIL: ({c[0]}, {c[1]}, {c[2]}): '{pag}' != '{c[3]}'")
		else:
			print(f"success: ({c[0]}, {c[1]}, {c[2]}): '{pag}'")

