# MAKEFILE
#
# @author      Nicola Asuni <nicola.asuni@genomicsplc.com>
# @link        https://github.com/genomicsplc/variantkey
# ------------------------------------------------------------------------------

# List special make targets that are not associated with files
.PHONY: help test build format clean

# --- MAKE TARGETS ---

# Display general help about this command
help:
	@echo ""
	@echo "VariantKey Javascript Makefile."
	@echo "Requires: node-js and uglify-js."
	@echo "The following commands are available:"
	@echo ""
	@echo "    make test    : Run the unit tests against source code"
	@echo "    make build   : Build and test a minified version of the library"
	@echo "    make format  : Format the source code"
	@echo "    make clean   : Remove any build artifact"
	@echo ""

all: clean format test build

# Run the unit tests
test:
	cd test && node test_variantkey.js '../src/variantkey.js'

# Build a minified version of the library
build:
	@mkdir -p target/build
	uglifyjs --compress --keep-fnames --comments --output target/build/variantkey.js src/variantkey.js 
	cd test && node test_variantkey.js '../target/build/variantkey.js'
	cd test && node test_regionkey.js '../target/build/variantkey.js'
	cd test && node test_esid.js '../target/build/variantkey.js'

# Format the source code
format:
	js-beautify --replace src/variantkey.js
	js-beautify --replace test/test_variantkey.js

# Remove any build artifact
clean:
	rm -rf target
