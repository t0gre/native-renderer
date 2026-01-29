#!/bin/bash

# Copy shaders from lib/shaders to Debug/shaders

chmod -R +x . 
cp -r ../lib/shaders .

echo "Shaders copied to $PWD/shaders"
