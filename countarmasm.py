import sys
from collections import Counter

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
	useful = set()
	for y in equivalences.values():
		useful = useful | set(y)
	print equivalences
	counters = None

	#vmrs
	for x in o:
		x = x.strip()
		if inside is None:
			if x in starts:
				inside = x
				counters = Counter()
				for q in useful:
					counters[q] = 0
				ignored = Counter()
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
		else:
			a = x.replace("\t"," ").split(" ")
			what = a[0]
			if what in skipped:
				continue
			if a[0] == "bl":
				what = a[1]
			if what in useful:
				counters[what] += 1
			else:
				ignored[what] += 1




