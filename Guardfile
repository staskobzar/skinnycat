# Guardfile
#
# Automatically runs tests when *.h or *.c files altered
# and runs "make" when Makfiles altered.
#
guard :shell do
  watch(/(.+)\.(c|h)/) do
    `make test`
  end

  watch(/.*\.exp$/) do
    `make`
    `make e2e_tests`
  end

  watch(/Makefile/) do
    `make clean && make`
  end
end
