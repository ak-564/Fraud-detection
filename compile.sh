#!/bin/bash

# Fraud Detection System Compilation Script
echo "🔧 Compiling Fraud Detection Backend..."

# Check if GCC is installed
if ! command -v gcc &> /dev/null; then
    echo "❌ GCC is not installed. Please install GCC to compile the C backend."
    exit 1
fi

# Compile the C program
gcc -o fraudBackend src/backend.c

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "✅ Backend compiled successfully: fraudBackend"
    echo "🚀 You can now run: node src/server.js"
else
    echo "❌ Compilation failed. Please check for errors."
    exit 1
fi

# Make executable
chmod +x fraudBackend
