	HowdySP_TP	A hackable exploration of Numenta's cortical learning algorithms

This is a slightly enhanced version of the Numenta NuPIC open source C++ example HelloSP_TP.cpp that can be found in several of their distributions online. Visit www.numenta.org for more information and links to their code.

The Numenta NuPIC open source project provides various opportunities to explore computer models of learning and processing algorithms that are based on recent analysis of the neuroanatomy of the mammalian neocortex. Much of the NuPIC code is written in Python, but the fundamental algorithms are also available in C++. Pure Python or a Python binding to the C++ libraries is the development approach for most of the application code in the Numenta open source project. Only a few examples are provided in C++, including one called HelloSP_TP.cpp, which introduces basic configuration of the spatial pooler (SP) and the temporal pooler (TP) classes. My extensions to that Numenta example became HowdySP_TP.cpp.

This program was motivated by my desire to display more information generated by the running example, and to vary the configuration parameters of the major classes it uses. It is not a ready made app, or even a demo program. I edit the files and rebuild it frequently, and I expect that other users of the code will operate in the same mode. That said, I don't think you have to be an experienced programmer to get something out of running it and varying the parameters from the command line. This example is mostly about the code, however, and more understanding of the Numenta code in the context of cortical learning is the goal.

The Numenta codebase and documentation are mostly Python-centric. I found separating the jungle surrounding the C++ core to be a bit frustrating. I'm working in a fairly generic Linux environment and compiling with Gnu C++ from a make file or the command line. Just getting to the point where I could build and run the example programs took longer than I expected - mostly time wasted. I offer this code as a point of departure for poking around the Numenta code and getting a sense of what it is and it isn't.

Bottom Line

HowdySP_TP is a small program with only a command line interface. This code assumes that you have the Numenta _algorithms.so and _engine_internal.so available. If you don't have that, you might look at my "Cheater's Guide" which offers a fairly simple way to get there. I've included a (very) simple makefile for that environment called HowdySP_TP.m (make -f HowdySP_TP.m). Since the Numenta libs are .so, they need to be accessible at run time, too. Again, see the "Cheater's Guide" for a simple fix. If you are using the environment described in the "Cheater's guide" you may find the two commands in Setup.sh (source Setup.sh) helpful.

Command Line Params

Several parameters can be set from the command line at run time. Take a look at CmdLineParse.cpp in the function InitTables(). You can just add another line there and access it from the main program pretty simply. For example, setting the RNG seed from the CLI --RngSeed=42 (or whatever) looks like this in the code:

  srand(CmdLine->Vars("RngSeed"));	// default is zero

I add and delete these often when I find something I want to mess with.

The CLII variables defined in this code are a core set that I use a lot:

  --alternate_reverse is a flag that reverses the order of the input on alternate passes (ok, ok)

  --dump_final flag enables a dump of the final SP and TP out buffers as per the original code

  --sp_summary flag prints the final SP and TP out buffers as lists of location : run length

  --tp_summary flag prints the final TP and TP out buffers as lists of location : run length

  SpVerbosity variable sets the Numenta verbosity level in the SP

  TpVerbosity variable sets the Numenta verbosity level in the TP

  Epochs variable sets the number of passes through the SP/TP compute loop (default 10000)

  RngSeed variable sets the value passed to srand() for the data generator. Same seed, same data.


Changes to the Example Code

The function of the Numenta example code is nearly intact. It looks different because I copied the comments about the class constructor parameters for the SP and TP classes into the main program text, and have added small features for setting the RNG seed, the epoch termination count, etc. I stripped this code down from what I've been using myself when I realized that it had significantly diverged from the example. I don't attempt to distinguish WTF territory from topics of interest to the NuPIC community at large. (All right, --alternate_reverse is odd and I left it in.)

Suggestions and Comments

When I first ran the HelloSP_TP example, I definitely placed it in WTF land. It doesn't do much and doesn't even like its data. After hacking at it for a while, I realized it's a great way to look at what's going on in the code. It becomes very absorbing. So, change the parameters around. It's hard to break it.

For example:

Use the command line to set the maximum epochs to 2000, so the runs take a few seconds and you can see the results right away.

./HowdySP_TP --sp_summary --tp_summary --RngSeed=6907 --Epochs=2000 --SpVerbosity=0 --TpVerbosity=0

Change the RNG seed around and the number of outputs stays stable but the locations move, even though the data may be radically different.

If you want to see more activity, don't increase the number of non-zero inputs. Even with a thousand bits of input, the original version consistently produces ten output runs. Change the SP parameter numActiveColumnsPerInhArea from its default value 10, to 20 instead. Now you'll see about 20 runs. The original example generated about 50% non-zero in the input, but HowdySP_TP generates about 2% at the expense of additional non-randomness. You can restore the old way.

Even with fewer non-zero inputs the internal actions churn. Use the command line variable TpVerbosity to get many more messages from the TP.

./HowdySP_TP --sp_summary --tp_summary --RngSeed=6907 --Epochs=2000 --SpVerbosity=0 --TpVerbosity=3

You'll see "Too much unpredicted input", "Trying to lock-on", "Failed to lock on. Falling back to bursting all unpredicted.", etc. in great profusion. At the beginning of the run the TP messages are mostly about created segments. At what point does it stop creating segments?


