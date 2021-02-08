# Introduction

This directory contains several manual deals used to test several Tarot use cases.
Each of them must be run before a TarotClub release to ensure that there is no 
any regressions.

# List and description of manual deals

This chapter describes the manual deal files available in the directory, how to use them and 
the expected results.

## Handle tests

  * 9_trumps_before_taking.json: announce a Guard during the bid sequence then build your discard. During 
  the first turn, the game must propose you to declare a handle because you have more than 10 trumps in your 
  hand (9 from the original deck + several in the dog).


