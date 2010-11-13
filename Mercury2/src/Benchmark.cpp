#include <Benchmark.h>
#include <stdio.h>
#include <stdlib.h>

void BenchmarkSSELoad()
{
	/*_mm_load_ps and _mm_loadu_ps seem to run at nearly the same speed
	_mm_loadu_ps 5.886555ns vs _mm_load_ps 5.564690ns
	*/
	MercuryTimer start;
	const unsigned long c = 1024*1024*20;
	FloatRow* d = new FloatRow[c];
	//d = (FloatRow*)(((char*)d)+4);
	printf("%d\n", ((unsigned long)d)%16);
	__m128 pp;
	unsigned long* idx = new unsigned long[c];
	for (int x = 0; x<c;++x)
	{
		unsigned int r = (unsigned int)rand();
		idx[c] = r%c;
	}

	double tu, ta;
	tu = ta = 0;
	for (int x = 0; x<10;++x)
	{
		start.Touch();
		for (unsigned int i = 0; i < c;++i)
		{
			pp=_mm_loadu_ps(d[i]);
			_mm_storeu_ps(d[i],pp);
		}
		tu += start.Touch()*1000000000; //to nano second
		for (unsigned int i = 0; i < c;++i)
		{
			pp=_mm_load_ps(d[i]);
			_mm_store_ps(d[i],pp);
		}
		ta += start.Touch()*1000000000; //to nano second
	}
	printf("loadu %f\n", tu/float(c)/10/2);
	printf("load %f\n", ta/float(c)/10/2);
	exit(0);
}