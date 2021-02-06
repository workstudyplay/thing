#!/bin/bash

# this will launch ngrok if it is avail, on error it will exit so only 1 instance will be allowed
start bash -c "exec ngrok http --subdomain=workstudyplay 3030"


