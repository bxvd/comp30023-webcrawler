# Web Crawler
**Assignment 1** COMP30023 Computer Systems

## Description
The goal is to have implemented the functionality of:
1. TCP communication - *done*
2. URL parsing - *done*
3. HTML parsing - *done*
4. Web crawling - *done*
5. Content-type and trancated response checking - *done*
6. Correctly respond to status codes 200, 404, 410, 414, 503, 504 - *done*
7. Redirection and authentication - *done*

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
