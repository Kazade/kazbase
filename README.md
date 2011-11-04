# kazbase C++ utility library

Kazbase exists to make my life easier when I'm coding C++ applications. It's basically a nice API wrapper around boost, the STL and other libraries like Curl. Many of the methods and classes are inspired by Python's standard library but adapted to better fit C++'s syntax and idioms.

# Examples

boost::filesystem provides a cross-platform way of manipulating paths, but the syntax for doing so isn't as nice as Python's equivalent os.path module. kazbase wraps boost::filesystem to produce the following syntax:

```
std::string p = os::path::join("/tmp", "my_file");
if(os::path::exists(p)) {
    os::delete_path(p);
}

os::touch(p);
datetime::DateTime d = os::stat::get_modified_time(p);
```

In the above example the datetime module is also used, datetime::DateType is just a typedef for boost's ptime.

There are loads of other wrappers for different things that I'll keep adding to for my own benefit, but feel free to use!
