//#pragma warning 549 10
void *operator new(size_t size,void *p)
{
	(void)size;
//	ASSERT(sizeof(T) == size);	// Must init over itself
	return p;
}

