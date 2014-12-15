#include <cassert>
#include <iostream>
#include <future>
#include <thread>
#include <exception>
#include <functional>
#include <atomic>

#include "message_queue.h"

int square ( const int x ) { return x*x; }
int twice  ( const int x ) { return 2*x; }
int increm ( const int x ) { return x+1; }

typedef std::function< int ( const int ) > Func;
typedef std::promise<int> Promise;
typedef std::shared_future<int>  Future;
typedef std::runtime_error Exception;

std::atomic<bool> done(false);

struct Work
{
	Work( Func f, Future arg ) : f_(f), arg_(arg) {}

	void operator()()
	{
	    try 
	    {
	    	arg_.wait();                     // wait for other computations
	    	int r = f_( arg_.get() );        // execute this computation
	    	// std::cout << r << std::endl;
	        result_.set_value(r);            // signal the result
	    } 
	    catch( ... ) 
	    {
	        result_.set_exception( std::current_exception() );
	    }
	}

	Future result() { return result_.get_future(); }

private:
	Promise result_;
	Future  arg_;
	Func f_;
};

void evaluate( message_queue< Work* >& mq )
{
	while( !done && !mq.empty() )
	{
		Work* w = NULL;
	    mq.wait_and_pop(w);
	    if(w) 
	    {
	    	(*w)();
		    delete w;
	    }
	    else
	    	return;
	}
}

#define N_WORKERS 10

int main()
{
	/// initialize

	message_queue< Work* > mq;

    std::vector<std::thread>  workers;
    for( size_t i = 0; i < N_WORKERS; ++i)
        workers.push_back( std::thread( evaluate, std::ref(mq) ) );

    /// enqueue 

    Promise s1; s1.set_value(7);

    Work* w1 = new Work( &square, s1.get_future() );
    Work* w2 = new Work( &increm, w1->result() );
    Work* w3 = new Work( &twice,  w2->result() );
    Work* w4 = new Work( &twice,  w3->result() );

    Future r = w4->result();

    mq.push( w4 );
    mq.push( w2 );
    mq.push( w1 );
    mq.push( w3 );

    while( !mq.empty() ) {}

    std::cout << r.get() << std::endl;

    // finalize
	
	done = true;

  	for( size_t i = 0; i < workers.size(); ++i )
    	mq.push(NULL);

  	for( size_t i = 0; i < workers.size(); ++i )
    	workers[i].join();
}