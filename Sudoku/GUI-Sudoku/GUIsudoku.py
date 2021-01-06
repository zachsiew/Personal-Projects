ADDY-HASH-SEARCH(T, k):
	i = 0
	temp = 0
	count = 1
	repeat
		if i == 0
			temp = 0
		else if i == 1
			temp = 1
		else
			count = count + 2
			temp = temp + count

		j = (h'(k) + temp) mod m
		if T[j] == k
			return j
		else
			i = i + 1
	until T[j] == NIL or i == m
	return -1