"Hello World" Example
=====================

Hello-Spider is a simple application where the main behavior of the web page is written in C++ code.
It is intended to show how the build system works.  The application itself is just a red square with
a black dot.  Clicking and dragging on the red square cause the black dot to follow your mouse location.

<b>Building Hello-Spider</b>

```
cd <wherever>/mutantspider-examples/hello-spider
npm install
make
```

This will require that you have the standard mutantspider build tools installed.  To run what you have
just built in local development web server execute:

```
make run_server
```

A pre-built vesion of this application can be found [here](http://www.mutantspider.tech/code/hello-spider/index.html).
