(* Copyright (C) 2024 Michael Herstine *)

Require Import Coq.Arith.PeanoNat.
Require Import Lia.

(** * Introduction *)

(** This is my attempted formalization of "The String-to-String
    Correction Problem" by Wagner & Fischer:

    Wagner, Robert A., and Michael J. Fischer. 1974. "The
    String-to-String Correction Problem." Journal of the Acm 21 (1):
    168-73. https://doi.org/10.1145/321796.321811.

    I started with Wagner & Lowrance, but found it to be difficult to
    formalize. Perhaps related is the fact that Ukkonen, the next step
    in the evolution of these algorithms, bases his work on Wagner &
    Fischer.

    This is my first non-trivial Coq development, so it's going to be
    quite chatty as I figure things out. *)

(** * Preliminaries *)

(** Throughout, let [a] & [b] be strings. While Wagner & Fischer use
    capital letters for their strings, that conflicts with Coq naming
    conventions, so I'm going to switch to lower-case. I first thought
    to use the stdlib [string] to represent them, but found that List
    is much more featureful, so instead I'll just use a List of Ascii.
    *)

Require Import Ascii.

Open Scope char_scope.
Definition c := "c".
Definition a := "097".
Print a. (** :=> "a" : ascii-- 'a' is ASCII code 97 *)

Close Scope char_scope.
Reset c.

(** I've played a bit with [Vector.t], but like others found it a bit
    tedious to work with. John Wiegley has some interesting
    alternatives here
    <https://stackoverflow.com/questions/42302300/which-vector-library-to-use-in-coq>,
    but for now let's just use lists: *)

Require Import List.
Import ListNotations.

Definition string := list ascii.

Compute ["H";"e";"l";"l";"o"].
