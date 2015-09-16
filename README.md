# GXLib

[GLib](https://developer.gnome.org/glib/) is a wonderful library that
provides (among others) many of the basic data structures one needs
when programming in C, such as lists and hash-tables.

GXLib is a library that provides extensions to GLib that I'm using for
my own programs. Since it depends on GLib 2.x (2.38 or later), GXLib's
versioning also starts with 2.x.

So far, there are a number of extensions fo `GList` that allow for
using it with a functional flavor, and takes inspiration from
[Mozilla's Rust](https://www.rust-lang.org) and
[Scheme's SRFI-1](http://srfi.schemers.org/srfi-1/srfi-1.html) and the
way they allow for solving problems such as those from
[Project Euler](https://projecteuler.net).

# Documentation

All functions have been documented, many with examples. See
[here](http://www.djcbsoftware.nl/code/gxlib/).

# Examples

Here are some examples. If you're not familiar with ``map`` and
``fold`` etc., it might look a little bit bewildering at first,
esp. since you can probably think of a way to do it with an explicit
loop.

However, after getting used to them, they provide a very natural and
clean way to express solutions to problems. And not only for the kind
of mathematical puzzles that are so useful for small examples.

## Product of primes

Let's take the product of the prime numbers up to 20. We could make a
one-liner, but let's do it step by step.

First, create a list of numbers 1..20, using ``gx_list_iota``:
``` c
gint prod;
GList *nums;
nums = gx_list_iota (20 /*number*/, 1/*start*/, 1/*step*/);
```

Now, we filter out any non-prime numbers. And we do it in-place (ie.,
changing the list we have, rather than making copy).

``` c
nums = gx_list_filter_in_place (nums, (GXPred)gx_is_prime, NULL, NULL);
```

finally, we take the product:

``` c
prod = gx_list_product (nums); /* 9699690 */
g_list_free (nums);
```

# Contributing

`GXLib` is a young library, so there is ample opportunity for adding
more functionality.... contributions are welcome! Here are some
guidelines:

- Follow the existing coding style; like `GLib` itself, `GXLib` uses
  the GNU-coding style. It looks a bit unusual in the beginning, but
  one gets used to it quickly. In Emacs: `C-c . gnu RET`.
- We strive to keep the code-coverage for the tests at 100%. So for
  each function, there needs to be one or more unit tests (in the
  `tests/` directory).
- When documenting things, follow the `gtk-doc` style already in
  use. If you add example code to the documentation, add the code
  snippet to `test-examples.c` as well, so they can be unit-tested,
  too. Always nice if the example code keeps working!




