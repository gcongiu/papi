/* This file is here just for consistency as for some CPU architecture
 * we need to specify a cpu component while for others we don't. To
 * avoid handling CPU architectures inside source files using guards
 * we create a link file that points to the right cpu_component. This
 * file handles the cases in which the CPU architecture does not
 * require a cpu component */
