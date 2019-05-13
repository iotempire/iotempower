# Makefile for IoTempower documentation
#
#
# check IoTempower is active
ifeq ($(IOTEMPOWER_ROOT),)
  $(error IOTEMPOWER_ROOT is not active, aborting)
endif


# You can set these variables from the command line.
SPHINXOPTS    = -Q
SPHINXBUILD   = sphinx-build
BUILDDIR      = "$(IOTEMPOWER_LOCAL)/doc"
SOURCEDIR     = "$(BUILDDIR)/_src"

# Put it first so that "make" without argument is like "make help".
help:
	@$(SPHINXBUILD) -M help "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)

.PHONY: help Makefile

# Catch-all target: route all unknown targets to Sphinx using the new
# "make mode" option.  $(O) is meant as a shortcut for $(SPHINXOPTS).
# TODO: remove links when calling make clean
%: Makefile
	@ mkdir -p "$(SOURCEDIR)"
	@ ln -n -s -f "$(IOTEMPOWER_ROOT)/doc" "$(SOURCEDIR)/doc"
	@ ln -s -f "$(IOTEMPOWER_ROOT)/doc/conf.py" "$(SOURCEDIR)/conf.py"
	@ ln -s -f "$(IOTEMPOWER_ROOT)/doc/index-doc.rst" "$(SOURCEDIR)/index-doc.rst"
	@ mkdir -p "$(BUILDDIR)"
	@$(SPHINXBUILD) -M $@ "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)
