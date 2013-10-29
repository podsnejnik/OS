
import random
import os
import sys

global numbers
numbers = []
def my_m():
	if random.random() >= 0.5: #well
		return 0
	else:
		return -1

def my_read(filename):
	handle = open(filename, 'r')
	for line in handle:
		if (my_m() == 0): # Bad file descriptor
			for j in line.split():
				try:
					if (my_m() == 0): # No space left on device
						numbers.append(int(j))
					else:
						print "MemoryError(28) - No space left on device"	
				except ValueError, e:
					print "ValueError in line ", j, "- doesn't int"
		else: 
			print "Can not open file!", filename, "to read ", "I/O error(9): Bad file descriptor"	
def main():
	if len(sys.argv) < 2:
			print "Use this format:\nour_osi2 filename1/directory [filename2/directory [filename3/directory [...]]"
	for i in xrange(1, len(sys.argv)):
			try:
				filename = sys.argv[i]
				path = filename
								
				if os.path.isdir(path):
					# print path, ' is directory'
					for root, dirs, files in os.walk(path): 
						for name in files:
							fullname = os.path.join(root, name)
							my_read(fullname)	
				else:						
					# print path, ' is file or nothing'
					if (my_m() == 0): # don't open file, file does't exist!
						if (my_m() == 0): # permission denied!
							my_read(filename)
						else:
							print("Can not open file!", filename, "to read", " Permission denied")
					else:
						print("Can not open file!", filename, "to read", " No such file or directory")
								
			except IOError, e:
				print "Can not open file!", filename, "to read ", "I/O error({0}): {1}".format(e.errno, e.strerror)
			except MemoryError, e:
				print_err("Out of memory by file {0}!".format(filename))
				raise e
			except Exception, e:
				raise e	
	print 'Numbers:\n', str(sorted(numbers))		
main()

