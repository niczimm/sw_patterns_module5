To build the C++ code:
- Install cmake for your system.
- Make a build directory, e.g., "mkdir ../build".
- Change to the build directory and run cmake, e.g., "cmake <path to src>".
- Build the executable.  On *nix this means run "make" in the build directory.

To run the C++ code:
- In the build directory run "./Patterns" to see the options.
- For example "./Patterns t Test1.xml".

To build the Java code:
- Make an output directory, e.g., "mkdir ../classes".
- Run javac.  On *nix this means something like
  "javac -d ../classes/ `find edu/ -name \*.java`".


To run the Java code:
- Each of the three classes in the patterns_class directory conatins a main()
  function.
- Run it with a command like
  "java -classpath ../classes edu.jhu.apl.patterns_class.XMLTokenizer Test1.xml".
