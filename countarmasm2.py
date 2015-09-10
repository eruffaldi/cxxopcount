import sys
from collections import Counter

class Block:
	def __init__(self,line):
		self.line = line
		self.label = ""
		self.prev = []
		self.next = None # same as targetblock
		self.exitmode = ""
		self.target = ""
		self.targetblock = None
		self.counters = Counter()
	def __str__(self):
		return "Block(%d)" % self.line
	def __repr__(self):
		return "Block(%d)" % self.line
if __name__ == '__main__':

	o = open(sys.argv[1],"r")

	starts = set(["fkd:","USGDC:"])
	inside = None
	equivalences = dict([
		("tri",("cos","sin")),
		("div",("vdiv.f32",)),
		("mul",("vmul.f32","vnmul.f32")),
		("mac",("vmls.f32","vmla.f32","vnmla.f32","vnmls.f32")),
		("add",("vsub.f32","vadd.f32")),
		("cmp",("vcmpe.f32",)),
		("sqrt",("sqrt","vsqrt.f32")),
		("load",("vldr","vstr","vmov.f32","vmrs"))])
	skipped = set([".fnstart","movt","movw"])
	condbranches = set("bne bhi bhi.w bgt.w blo".split(" "))
	useful = set()
	for y in equivalences.values():
		useful = useful | set(y)
	print equivalences
	counters = None

	#vmrs
	line = 0
	blocks = []
	current = None
	labels = {}
	last = ""
	for x in o:
		line = line + 1
		x = x.strip()
		if inside is None:
			if x == ".fnstart":
				inside = last[0:-1]
				counters = Counter()
				for q in useful:
					counters[q] = 0
				ignored = Counter()
				blocks = []
				current = Block(line)
				blocks.append(current)
				labels = {}
		elif x == ".fnend":
			# TODO equivalence classes 
			print inside
			print "--instructions"
			print "\n".join(["\t%s:%3d" % (k,v) for k,v in counters.iteritems()])
			oo = Counter()
			print "--classes"
			for k,v in equivalences.iteritems():
				n = 0
				for x in v:
					n += counters[x]
				print "\t",k,n
			print "--ignored","\n","\n".join(["\t%s:%3d" % (k,v) for k,v in ignored.iteritems()])
			inside = None
			for b in blocks:
				if b.exitmode == "branch":
					b.targetblock = labels[b.target]
					b.next = b.targetblock
					b.targetblock.prev.append(b)
			# remove unreachable and special
			blocks = [b for b in blocks if (b != blocks[0] or len(b.prev) > 0) and not b.label.startswith(".LCPI0")] # not fake
			for b in blocks:
				print "block at",b.line,b.label,b.exitmode,"target:",b.target,"from:",[q.line for q in b.prev],"\n","\n".join(["\t%s:%3d" % (k,v) for k,v in b.counters.iteritems()])

			# follow ALL paths from root to exit nodes (or viceversa)
			root = blocks[0]
			paths = [[b] for b in blocks if b.exitmode == "return"]
			result = []
			print "wayouts",paths
			while len(paths) > 0:
				nextpaths = []
				for p in paths:
					if p[0] == b:
						print "root",p[0]
						result.append(p)
					elif len(p[0].prev) == 0:						
						print "strange non root",p[0]
						result.append(p)
					else:
						print "not root",p[0],len(p[0].prev)
						for c in p[0].prev:
							print "adding",c,"to",p
							nextpaths.append([c] + p)
				paths = nextpaths
				print "LOOP",len(paths)
			print "result for root",blocks[0]
			for x in result:
				total = Counter()
				for c in x:
					total.update(c.counters)
				print x,"\n","\n".join(["\t%s:%3d" % (k,v) for k,v in total.iteritems()])


		else:	
			x = x.replace("\t"," ")
			if x.endswith(":"):
				label = x[0:-1]
				# mark block
				newblock = Block(line)
				if current is not None:
					newblock.prev.append(current)
					current.next = newblock
				current = newblock
				current.label = label
				blocks.append(current)
				labels[label] = current
			elif x == "bx lr":
				# return instruction
				current.exitmode = "return"
				print "return at ",line," started",current.line
				current = None
			else:
				a = x.split(" ")
				what = a[0]
				if what in condbranches:
					# conditional branch means keep current (next is a label)
					current.target = a[1]
					current.exitmode = "branch"
					print "condbranch at ",line," started",current.line," to ",current.target
				elif what == "b":
					# branch (next is empty)
					current.target = a[1]
					current.exitmode = "branch"
					print "branch at ",line," started",current.line," to ",current.target
					current = None
				elif what in skipped:
					pass
				elif a[0] == "bl":
					print "!!!UNSUPORTED bl"
					what = a[1]
				elif what in useful:
					counters[what] += 1
					current.counters[what] += 1
				else:
					ignored[what] += 1
		last = x




