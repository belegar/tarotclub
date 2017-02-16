# ICL: Independant C++ Library

ICL provides a set of (nearly) non interdependant utility classes that fill the missing standard C++ library features or wrap OS specific stuff.

## Synchronization with ICL subdirectories

### Add Subtree
git subtree add --prefix src/icl https://github.com/arabine/icl master --squash

### Update Subtree
git subtree pull --prefix src/icl https://github.com/arabine/icl master --squash

### Push changes

Now we can use the subtree push command like the following:
git subtree push --prefix src/icl https://github.com/arabine/icl master

