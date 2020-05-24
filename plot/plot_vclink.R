argv = commandArgs(T)

vclink_file = argv[1]
out_prefix = argv[2]

vclink = read.table(vclink_file, header = T, check.names = F, sep = "\t", stringsAsFactors = F, na.strings = c("NA", "nan", "-nan"),  comment.char = "")

#offsets for hg19
chr_offset = cumsum(c(0,248956422,242193529,198295559,190214555,181538259,170805979,159345973,145138636,138394717,133797422,135086622,133275309,114364328,107043718,101991189,90338345,83257441,80373285,58617616,64444167,46709983,50818468,156040895,57227415,16569))

linkagePlot = function(y_vals, ...){
	plot(NA, xlim = c(0, chr_offset[22]), ylim = c(min(0, y_vals, na.rm = T), max(y_vals[is.finite(y_vals)], na.rm = T)), xlab = "Chromosome",  xaxt="n", ...)
	axis(1, at=(chr_offset[1:22]+chr_offset[2:23])/2, labels = 1:22)
	abline(v = chr_offset, lty=3)
	for (chr in unique(vclink$chr)){
		index = which(vclink$chr == chr)
		points(chr_offset[chr] + vclink[index, "pos"],y_vals[index], lwd = 2, col = "blue", type="l")
	}
}

# chr	pos	sigma2_K	sigma2_IBD	sigma2_e	pve_K	pve_IBD	se_sigma2_IBD	zstat	pvalue	lod

pdf(paste0(out_prefix, ".zstat.pdf"), width=7/480*950, height=7/480*400) ; linkagePlot(vclink$zstat, ylab="Z statistic") ; dev.off()
pdf(paste0(out_prefix, ".neglog10pvalue.pdf"), width=7/480*950, height=7/480*400) ; linkagePlot(-log10(vclink$pvalue), ylab=expression(-log[10](p-value))) ; dev.off()
pdf(paste0(out_prefix, ".lod.pdf"), width=7/480*950, height=7/480*400) ; linkagePlot(vclink$lod, ylab = "LOD") ; dev.off()
pdf(paste0(out_prefix, ".sigma2_IBD.pdf"), width=7/480*950, height=7/480*400) ; linkagePlot(vclink$sigma2_IBD, ylab = expression(paste(sigma^2, "  IBD"))) ; dev.off()
pdf(paste0(out_prefix, ".pve_IBD.pdf"), width=7/480*950, height=7/480*400) ; linkagePlot(vclink$pve_IBD, ylab = "Prop. Variance Explained IBD") ; dev.off()
pdf(paste0(out_prefix, ".pve_K.pdf"), width=7/480*950, height=7/480*400) ; linkagePlot(vclink$pve_K, ylab = "Prop. Variance Explained K") ; dev.off()
pdf(paste0(out_prefix, ".pve_K_IBD.pdf"), width=7/480*950, height=7/480*400) ; linkagePlot(vclink$pve_IBD + vclink$pve_K, ylab = "Prop Variance Explained") ; dev.off()
