# CSCI-389 Homework #4: "Let's Network"
## By Maxx Curtis and Casey Harris

### Preparation:
    - We used our own code from HW3 as a baseline for this assignment. We fixed a few key errors we had found
    with our code before beginning (such as our tests only passing single characters, instead of testing the
    full capabilities of passing strings).


### Server:
    - We used Boost::Beast to create both our server and client. The server is asynchronous, and is modeled
      off of the example asynchronous server from Boost's documentation.
      https://www.boost.org/doc/libs/1_72_0/libs/beast/example/http/server/async/http_server_async.cpp

    - We realized pretty late into the project that GET should return a JSON tuple, so we ended up returning
      strings with proper JSON formatting instead. We hope that's not too big an issue.

    - Also for GET, we struggled to understand how to pass both the relevant information from the JSON tuple
      as well as a pointer to the size necessitated by the call to get() on the client side, so we added an
      additional pair to the server's output, which the client then referenced to set the pointer. Thus, the
      pointer for get() is entirely handled on the client side, so it is the client's role to handle that
      memory.

    - GET returns the string "NULL" as its body in the event that a key is not found. It still returns an
      ok status.

    - Similarly, DELETE returns the string "TRUE" or "FALSE" based on whether the key was successfully deleted
      or not. Both cases return an ok status.
    
    - POST returns a not found status if the data it receives is anything other than "/reset".

    - Our default host is 127.0.0.1 and the default port is 3618, though these options can be overridden on
     the command line.

    - We had trouble with linker errors that we could not determine the source of when we had an evictor
      for our cache, so we didn't run any tests requiring an evictor. The default option (simply disallowing
      sets when the cache was too full) worked fine.


### Client:
    - Similarly, the client wa based off of the synchronous client example from Boost's documentation:
      https://www.boost.org/doc/libs/1_72_0/libs/beast/example/http/client/sync/http_client_sync.cpp

    - Our cache client is implemented using pImpl, whose constructor only passes on host and port.

    - All member functions create their own tcp_stream object which is written to and read from once before being 
      closed. This isn't ideal, but when trying to do multiple operations on the same stream we recived unexpexted 
      "end of stream" errors. The major differences between function calls have to do with the construction of the 
      request, and the way they handle the response.

    - Impl currently has only a default deconstructor due to unexplained segmentation faults when it only called 
      pImpl->reset(). We have verified via valgrind that the lack of a deconstructor does not lead to any memory 
      leaks, however this could be problematic for test code that anticipates a complete cache deconstruction/
      reconstruction between tests.


### Test_Cache_Client:
    - This file is, essentially, a copy-paste of test_cache, modified to use the networked cache constructor. All
    of the tests and calls are identical to the non-networked version.

    - We had a strange bug where our test_reduction() test (which checks if a cache correctly updates the value and 
    size associated with a key after set is called on a key that already exists, using a value of smaller size than 
    the original) would fail unless it was the first test in the sequence. We're not sure why this bug happens, but 
    we are aware of it. To demonstrate that all of our tests can pass, we have placed that test first in main.

    - All of the tests that would require invoking an evictor have been commented out. This is due to the linker
    error resulting from including evictors, rather than failure to make the contents of those tests conform to
    desired behavior. The tests were taken directly from the non-networked version of the cache, which did pass all 
    but two.
    
