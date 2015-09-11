# ARM Assembler Analyzer
import sys
from collections import Counter

armistr2cycle = {"vmla.f32":1,"cmp":1,"vmul.f32":1,"vldr":1,"vmov.f32":1,"vadd.f32":1,"vsqrt.f32":14,"vdiv.f32":14,"vnmls.f32":1,"vmls.f32":1,"vsub.f32":1,"vmrs":1,"vcmpe.f32":1,"cos":14,"vnmla.f32":1,"sqrt":14,"vstr":1,"vnmul.f32":1,"sin":14}

def stmt2cycles(x):
	aa = armistr2cycle
	return sum([b*aa[a] for a,b in x.iteritems()])

class Path:
	def __init__(self,pa,pc,t):
		self.path = pa
		self.counter = pc
		self.total = t
	def __str__(self):
		return "Path(lastline=%d,blocks=%d,total=%d)" % (self.path[-1].line,len(self.path),self.total)
	def __repr__(self):
		return self.__str__()


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
		return self.__str__()

class Function:
	def __init__(self,line,name):
		self.line = line # start line 
		self.endline = line # end line
		self.name = name 
		self.blocks = None # list of blocks, first is root / entry
		self.labels = None # labels
		self.paths = [] # paths as array of blocks from root to exit point
		self.counter = None # overall ops (sum of all blocks)
		self.ignored = None # ignore all ops
		self.total = 0
		self.worst = 0
	def __str__(self):
		return "Function(%s,%d-%d,blocks %d,paths %d,worst %d)" % (self.name,self.line,self.endline,len(self.blocks),len(self.paths),self.worst)
	def __repr__(self):
		return self.__str__()

def analyze(name):
	o = open(name,"r")

	functions = {}
	inside = None
	equivalences = dict([
		("cmpi",("cmp",)),
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
	counters = None

	#vmrs
	line = 0
	blocks = []
	current = None
	labels = {}
	last = ""
	fx = None
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
				fx = Function(line,inside)
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
						#print "root",p[0]
						result.append(p)
					elif len(p[0].prev) == 0:						
						#print "strange non root",p[0]
						result.append(p)
					else:
						#print "not root",p[0],len(p[0].prev)
						for c in p[0].prev:
							#print "adding",c,"to",p
							nextpaths.append([c] + p)
				paths = nextpaths
				#print "LOOP",len(paths)

			print "result for root",blocks[0],"total blocks",len(blocks)
			pc = []
			for x in result:
				total = Counter()
				for c in x:
					total.update(c.counters)
				pp = Path(x,total,stmt2cycles(total))
				pc.append(pp)
				print pp,"\n","\n".join(["\t%s:%3d" % (k,v) for k,v in total.iteritems()])

			fx.endline = line
			fx.blocks = blocks
			fx.labels = labels
			fx.paths = pc
			fx.counter = counters
			fx.ignored = ignored
			fx.total = stmt2cycles(fx.counter)
			fx.worst = max([p.total for p in fx.paths])
			functions[fx.name] = fx
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
	return functions


if __name__ == '__main__':
	q = analyze(sys.argv[1])
	for k,v in q.iteritems():
		print "\t",v

	q0 = q.values()[0]
	if q0 == "__ieee754_asinf":
		pass

	#LBB0_7: [> 0.975]
	#LBB0_9: [0.5,0.975]