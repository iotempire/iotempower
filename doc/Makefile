# Makefile for UlnoIoT documentation
#
#
# check ulnoiot is active
ifeq ($(ULNOIOT_ROOT),)
  $(error ULNOIOT_ROOT is not active, aborting)
endif


# You can set these variables from the command line.
SPHINXOPTS    = -Q
SPHINXBUILD   = sphinx-build
BUILDDIR      = "$(ULNOIOT_LOCAL)/doc"
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
	@ ln -n -s -f "$(ULNOIOT_ROOT)/doc" "$(SOURCEDIR)/doc"
	@ ln -s -f "$(ULNOIOT_ROOT)/doc/conf.py" "$(SOURCEDIR)/conf.py"
	@ ln -s -f "$(ULNOIOT_ROOT)/doc/index-doc.rst" "$(SOURCEDIR)/index-doc.rst"
	@ mkdir -p "$(BUILDDIR)"
	@$(SPHINXBUILD) -M $@ "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)
