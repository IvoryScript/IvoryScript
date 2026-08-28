# IvoryScript

> The programming language for [The Ivory System](https://ivorysystem.net).

IvoryScript is a declarative programming language designed for dynamic programming and data storage.
With clean Haskell-inspired syntax and a small core, it provides precise control over when values are reduced —
making it well-suited to both application logic and persistent data modelling.

> *"Computer science has two and precisely two problems: the naming of values, and the time at which a value is bound to a name."*
> — Maurice Wilkes, Cambridge University, c. 1976

A programming tool for both — and an expansion of the second: the time at which a value is reduced to a simpler form.

---

## Features

| Feature | Description |
|---|---|
| **First-class names and types** | Efficient handling of dynamically typed values and binding names |
| **On-demand data** | Separate data types for lazy expression values |
| **No implicit evaluation** | Full explicit control over when expressions are reduced |
| **Pattern matching** | Unrestricted data representation using constructor and deconstructor functions |
| **Pragmatic update** | Controlled mutability through reference values |
| **Multiple environments** | Native ability to interact with multiple environments |
| **Persistence** | Binary serialisation for all general data types |

---

## Quick start

> IvoryScript is currently in **beta**. Syntax and runtime may change before the stable 1.0 release.

### Try it online

The fastest way to get started is the **[IvoryScript online console](https://ivoryscript.net/console)** — no installation needed. Each session provides an independent sandbox for script entry and execution.

### Hello, world

```IvoryScript
"Hello, world"
```

Or using string concatenation

```i
"Hello" ++ ", world"
```

Or via the `Show` class method

```IvoryScript
show "Hello, world\n"
```

Or from a list

```IvoryScript
string (reverse ['d','l','r','o','w',' ',',','o','l','l','e','H'])
```

### A taste of IvoryScript

**Type declaration** 

**Function definition**

**Function application** — no parentheses or commas needed:

```IvoryScript
let {
   greet :: String -> String;
   greet name = "Hello, " ++ name ++ "!"
} in
   greet "IvoryScript"
-- Hello, IvoryScript!
```

**Pattern matching with `case`**

```IvoryScript
let {
   describeNumber :: Int -> String;
   describeNumber n = case n of {
      0 -> "Zero";
      1 -> "One";
      otherwise -> "Other"
   }
} in
   describeNumber 1
-- One
```

**Nested patterns**


```IvoryScript
case [(1, "one"), (2, "two"), (3, "three")] of {
    (n, text) :+ rest -> (text, n, rest);
    []                -> ("empty", 0, []);
}
```

**Currying and partial application**

```IvoryScript
let {
   add :: Int -> Int -> Int;
   add x y = x + y;

   add3 :: Int -> Int;
   add3 x = add x 3   -- or: add3 x = (+) x 3 
} in
   add3 5
-- 8
```

**Properties by name and type**

```IvoryScript
let ps :: PropertySet = PropertySet [
    name:      "temperature sensor",
    retries:   3,
    active:    True,
    threshold: 18.5,
    count:     42
] in {
   show (mapf (\p -> nameOfProperty p)  (properties ps)); show '\n';
   filter (\p -> (typeOfProperty p) = #::Int)  (properties ps)
}
-- [name,retries,active,threshold,count]
-- [retries:3,count:42]
```

**On-demand data** — work with potentially infinite sequences

```IvoryScript
let {
   listFrom :: Int -> Int -> Exp [Int];
   listFrom s step = s :+ (listFrom (s + step) step);
   evenWholeNumbers = listFrom 0 2
} in
   take 10 evenWholeNumbers
-- [0,2,4,6,8,10,12,14,16,18]
```

**Controlled mutability via pointers**

```IvoryScript
let x :: Ptr Int = ^0 in {
   show (@x); show '\n';  -- 0
   x @= 42;
   show (@x); show '\n';  -- 42
}
```

State dependent value

```IvoryScript
let {
   mkCounter :: Int -> Int -> Exp Int;
   mkCounter i step =
      let {
         countPtr :: Ptr Int = ^i;
         thunk :: (-> Int);
         thunk = variable (\-> let count :: Int = @countPtr in {countPtr @= count + step; count});
      } in
         fromThunk thunk;
} in {
   def .counter1 = Any #!(mkCounter 10 1);
   def .counter2 = Any #!(mkCounter 100 (-1))
};
.counter1;
.counter2;
.counter1;
.counter2;
```

**Lazy persistence**

```IvoryScript
let {
   ts :: TransientStore = TransientStore;
   os :: OutputStream Byte = fileOutputByteStream "expDecay.isds"
} in {
  def ts.decayFrom100th = Any (#!(
     let {
        decay :: Double -> Exp [Double];
        decay x = x :+ decay (x * 0.9)
    } in 
       drop 100 (decay 1.0)));
   insert os ts;
   streamCloseOutputAction os;
   destroy ts;
};

let {
   is :: InputStream Byte = fileInputByteStream "expDecay.isds";
   ts :: TransientStore = #!(extract is)
} in {
   streamCloseInputAction is;
   show (take 100 #!(ts.decayFrom100th::Exp (Exp [Double]))); show '\n';
   destroy ts
}
```

> **Try:** rerun just the second part with different values of `take`.

---


## Command-line programs

Two command-line programs are included with IvoryScript.

### `iss`

`iss` provides an interactive Ivory Script/System session.

The accompanying `ivory` shell script starts `iss` with modules listed in a
configuration file before passing control to an Order script prompt.

For example:

```console
$ ./ivory std
> "Hello, world"
Hello, world
> sum (take 100 primes)
24133
> end
$
```

Linux builds are provided for Intel and ARM, tested on Fedora and Raspberry Pi.

### `ivorysc`

`ivorysc` provides more direct control of the IvoryScript compiler through command-line options. It supports non-interactive use and regression testing from a single test file.

Builds are provided for Linux on Intel and ARM, and as a Win32 command-line executable.

---

## Getting the source

Clone the repository:

```bash
git clone https://github.com/IvoryScript/IvoryScript.git
cd IvoryScript
```

Alternatively, download and extract the source archive from GitHub.

---

## Building

The existing source hierarchy has been retained so that paths correspond directly with the originating codebase and build system.

### Linux prerequisites

* `g++`
* 32-bit compiler and library support when building the Intel version on a 64-bit system

### `iss`

On Linux, from the repository root:

```bash
cd source/app/ivory/iss
make ARCH=32
```

The build has been tested on Fedora and Raspberry Pi.

### `ivorysc`

On Linux, from the repository root:

```bash
cd source/app/ivory/ivorysc
make ARCH=32
```

The build has been tested on Fedora and Raspberry Pi.

For Windows, open the Visual Studio solution:

```text
build\win32\app\ivory\ivorysc\ivorysc.sln
```

This builds `ivorysc` as a Win32 command-line executable.

### Regression tests

On Linux, from `source/app/ivory/ivorysc`:

```bash
./regressionTest
```

The script selects the build for the host architecture and runs:

```text
config/ivory/suites/regression.suite
```

with the results written to:

```text
data/ivory/logs/test.log
```

To select the 32-bit build explicitly:

```bash
IVORYSCRIPT_32=1 ./regressionTest
```

## Repository layout

The repository follows the existing Ivory code base rather than reorganising files under release-specific source, script, config and data directories.

```text
IvoryScript/
├── build
├── config
│   └── ivory
│       └── suites
├── data
│   └── ivory
│       ├── logs
│       └── test
│           └── regression
├── scripts
│   └── IvoryScript
│       ├── examples
│       ├── modules
│       └── test
│           ├── devel
│           └── regression
└── source
    ├── API
    │   ├── MSA
    │   ├── archive
    │   ├── gen
    │   ├── ivory
    │   │   ├── common
    │   │   └── compiler
    │   │       └── parser
    │   ├── nameTable
    │   ├── objStore
    │   └── segmentTable
    ├── app
    │   ├── ivory
    │   │   ├── iss
    │   │   ├── isss
    │   │   └── ivorysc
    └── header
        ├── Win32
        ├── ivory
        │   ├── compiler
        │   ├── unix
        │   ├── win32
        └── unix
```

Only the source paths required by IvoryScript and their shared support code are included.

---

## Documentation

Further documentation is available at [ivoryscript.net/documents](https://ivoryscript.net/documents), including the User Guide, Reference Manual and articles discussing the design and semantics of IvoryScript.


## Documentation

Full documentation is available at [ivoryscript.net/documents](https://ivoryscript.net/documents):

- **[Getting Started](https://ivoryscript.net/documents/getting-started)** — console access, basic examples, and core concepts
- **[Reference Manual](https://ivoryscript.net/documents/reference-manual)** — complete language specification: types, expressions, patterns, modules, and semantics

---

## Contributing

IvoryScript is approaching its stable release and welcomes early contributors.

1. **Fork** this repository
2. **Create** a feature branch (`git checkout -b feature/my-feature`)
3. **Commit** your changes (`git commit -m 'Add my feature'`)
4. **Push** and open a **Pull Request**

Please read [CONTRIBUTING.md](https://github.com/IvoryScript/.github/tree/main/profile/CONTRIBUTING.md) before submitting. All contributors are expected to follow our [Code of Conduct](https://github.com/IvoryScript/.github/tree/main/profile/CONTRIBUTING.md#code-of-conduct).

---

## 🔗 Related

- [IvorySystem](https://github.com/IvorySystem) — An application programming framework based on IvoryScript
- [ivoryscript.net](https://ivoryscript.net) — Official language website and documentation
- [Online Console](https://ivoryscript.net/console) — IvoryScript in a browser

---

## 📄 Licence

IvoryScript is released under the [MIT Licence](https://github.com/IvoryScript/.github/tree/main/LICENCE).