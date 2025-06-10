Compute Inventory
=================

This directory contains the implementation of the compute inventory
service. The service gathers basic information about the CPU and
available memory during early kernel initialization. Other subsystems
can query the global inventory record to tailor their behavior.
