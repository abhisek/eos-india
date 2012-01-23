 == The Memory Management Unit Simulator ==

 The system consists of the following components:

   * The Page Fault Handler
      * Implemented in pfh.c

   * The Page Aging Manager
      * Implemented in pam.c

   * The Access Request Handler
      * Implemented in arh.c

   * Information Dumper UI
      * Implemented in infodump.c

 The system reads initial Input from pmem.dat located in the current
 working directory. After initialization is done, the above mentioned
 components are started in their individual threads and the main thread
 becomes the "Information Dumper UI".

 The format of pmem.dat is as follows:
   * First line of the file contains 4 integersi: N M P F T R
      * N: The size of physical memory in frames (Frame count)
      * M: The size of virtual memory in pages (Page count)
      * P: Size of Page (Frame Size == Page Size)
      * F: Time Delay in Millisecond
      * T: Time Delay in Millisecond
      * R: Time Delay in Microsecond

  * The first line is followed by M x N matrix of integers representing
    the initial content of the Virtual Memory.

 The Access Request Handler processing all Virtual Memory read/write
 requests. In case of a Page Fault, it submits swap-in request to the
 Page Fault Handler request queue and reads appropriate response from
 the corresponding response queue.

 The Access Request Handler reads Input from req.dat located in current
 working directory.

 The format of req.dat is as follows:
   * Each Line represents a request.
   * Each request has four parameters: <Read/Write> <Page> <Offset> <Data>
   * The <Data> parameter is ignored in case of a <Read>

 The main thread after initializing other components becomes the information
 dumper thread and accepts the following commands from stdin:
   * <source> <sleep>
      * Where <source> is a 7 character string and <sleep> is delay in milliseconds.
      * <source> can be any one of the following:
         * memory- : To dump physical memory
         * process : To dump virtual memory
         * pagetbl : To dump Page Table
         * pfaultq : To dump Page Fault queues

 === How to Compile ===

 Type ``make`` in the source root

 === How to Run ===

 ./memory

 === Testing ===

 Tested over GNU/Linux using the provided pmem.dat and req.dat


