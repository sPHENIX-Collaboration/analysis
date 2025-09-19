import ROOT

# 打开文件
infile = ROOT.TFile.Open("outputFile/pt_relative_error_combined.root")
if not infile or infile.IsZombie():
    print("❌ Cannot open ROOT file!")
    exit(1)

# 读取 TH1D
hist1d = infile.Get("h_relerr_combined")
if not hist1d:
    print("❌ Cannot find h_relerr_combined!")
    infile.Close()
    exit(1)

# 创建画布
c1 = ROOT.TCanvas("c1", "RelErr Combined", 1000, 1000)

# 高斯拟合
fit_result = hist1d.Fit("gaus", "S", "", -0.1, 0.1)  # "S" 静默拟合
gaus_func = hist1d.GetFunction("gaus")
mean = gaus_func.GetParameter(1)
sigma = gaus_func.GetParameter(2)

# chi2 / ndf
chi2 = gaus_func.GetChisquare()
ndf = gaus_func.GetNDF()
chi2_ndf = chi2 / ndf if ndf != 0 else 0

# 打印
print(f"Gaussian fit result:")
print(f"  mean   = {mean:.4f}")
print(f"  sigma  = {sigma:.4f}")
print(f"  chi2/ndf = {chi2:.2f} / {ndf} = {chi2_ndf:.4f}")

# 画图
hist1d.Draw()
gaus_func.SetLineColor(ROOT.kRed)
gaus_func.SetLineWidth(2)
gaus_func.Draw("same")

# 加标注
latex = ROOT.TLatex()
latex.SetNDC()
latex.SetTextSize(0.04)
x_pos = 0.6
y_pos = 0.75
latex.DrawLatex(x_pos, y_pos,     f"#mu = {mean:.4f}")
latex.DrawLatex(x_pos, y_pos-0.05, f"#sigma = {sigma:.4f}")
latex.DrawLatex(x_pos, y_pos-0.10, f"#chi^{{2}}/ndf = {chi2_ndf:.4f}")

# 保存
c1.SaveAs("outputFile/h_relerr_combined_fit_new.png")

# 关闭文件
infile.Close()

print("✅ Fit plot saved to outputFile/h_relerr_combined_fit_new.png")
