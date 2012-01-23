#!/bin/bash

rdoc ./ruby-libnet.c
scp -r doc/* abhisek@rubyforge.org:/var/www/gforge-projects/ruby-libnet/
