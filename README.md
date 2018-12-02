# Fundamentals_of_Compiling

***Notice: This project is based on "CMMInterpreter", whose url is :https://github.com/hukun01/CMMInterpreter.*** 



## Grammar

![1](..\img\1.png)

![2](..\img\2.png)

## What's new?

### General

As the grammar shown, some statements are in different but equivalent forms. For example, the expression is not same as the old version. However there is no difference between old and new. To achieve the course requirements and keep a clam mind, I modify some terms such as *if-stmt*  to *sele-stmt* , and the grammar is same as shown above strictly.

### Function Detection

Frankly, this part is similar to the old version. Just add some rules about function. It's not difficult to understand.

### Local Variables and Scope of Variables

In general, the BFS is the key thought. 

Firstly, the "***Depth***" and "***Depthnum***" are introduced in attributes. 

The depth is the index of the code block which could have local variables.  Because of the recursion (logical), there are different depths. 

The depthnum is the index of the quantity of code block whose depth is depth.

OK, because of my poor English, let me explain these definition in the example below.

> Code 1

```
void func1() {

	if ( ... ) then {...}

}

void func2() {

	while (...) do {...}

}
```

In this example, func1 is the first block can have local variables. So I define the func1's depth is 1. There is no block before, so the depthnum of func1 is 1.

The func2 apposite to func1. So the func2's depth is 1 and the depthnum is 2.

The if statement in func1 has the depth 2 and the depthnum 1.

The while statement in fun2 has the depth 2 and the depthnum 2.

Ummm... Maybe I can suppose you have already known what I mean.



After we define the depth and depthnum, I add the depth and depthnum to the name to distinguish the same-named variables. 

But it brings a problem. We can't find the declaration in the symbol table directly like before. Because the name is following its domain. 

To solve this problem, a link indicating the relationship of domain is introduced. After we build the syntax tree, we do BFS on the tree to build the link. 

We can find the symbol table in this way: find the local domain, if there is no match result, find the previous domain using the link.