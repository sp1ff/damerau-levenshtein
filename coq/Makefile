# Taken from <https://github.com/coq-community/manifesto/wiki/Recommended-Project-Structure>
all: Makefile.coq
	@+$(MAKE) -f Makefile.coq all

clean: Makefile.coq
	@+$(MAKE) -f Makefile.coq cleanall
	@rm -f Makefile.coq Makefile.coq.conf
	@rm -rf doc

Makefile.coq: _CoqProject
	$(COQBIN)coq_makefile -f _CoqProject -o Makefile.coq

force _CoqProject Makefile: ;

DOCDIR := doc

doc:
	@test -d doc || mkdir doc
	coqdoc --html --utf8 -d doc/ theories/wf.v

%: Makefile.coq force
	@+$(MAKE) -f Makefile.coq $@

.PHONY: all clean force doc
