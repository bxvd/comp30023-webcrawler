# Web Crawler
**Assignment 1** COMP30023 Computer Systems

## Description
The goal is to have implemented the functionality of:
1. ~~Web crawling~~
2. ~~TCP communication~~
3. ~~HTML parsing~~
4. ~~URL parsing~~
5. Content-type and trancated response checking
6. Correctly respond to status codes 200, 404, 410, 414, 503, 504
7. Authentication

### Usage
```bash
$ make
$ ./crawler <URL>
```

## Authors
Brodie Daff

## Acknowledgements
* `Makefile` was adapted from http://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/
* `client.c` was adapted from COMP30023 workshop 4.
* Uses [Gumbo HTML Parser](http://github.com/google/gumbo-parser) by Google.
