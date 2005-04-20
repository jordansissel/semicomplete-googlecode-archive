

/* mutual exclusion with swap */

int key, lock = 0;
while (1) {

	/* Begin exclusion section */
	key = 1;
	while (key) 
		Swap(lock,key);

	/* 
	 * Critical Section 
	 */

	lock = 0;
	/* End exclusion section */

	/* 
	 * non-critical section
	 */
}
