#import libraries
import numpy as np

import matplotlib.pyplot as plt
from matplotlib import style
style.use("ggplot")

from sklearn import svm

import itertools

# get data - format: event PID momentum E_Ecal E_HcalInner
csv_e  = np.genfromtxt ('testdata/set001/CalorimeterResponseHijingCentralRapidity_e_8GeV.csv')
csv_pi = np.genfromtxt ('testdata/set001/CalorimeterResponseHijingCentralRapidity_pi_8GeV.csv')

# set pid column (NaN) to 0
csv_e[:,1] = 0
csv_pi[:,1] = 0

# separate Momentum, Ecal and Hcal energy columns from rest
data_e_sub = csv_e[:,2:5]
data_pi_sub = csv_pi[:,2:5]

# split data in train and test data
data_e_train = data_e_sub[0:500,:]
data_e_test  = data_e_sub[500:1000,:]

data_pi_train = data_pi_sub[0:500,:]
data_pi_test  = data_pi_sub[500:1000,:]

# merge electrons and pions
data_train = np.vstack((data_e_train,data_pi_train))
data_test = np.vstack((data_pi_test,data_e_test))

# pid integer: 0 = pion, 1 = electroon
id_pi = 0
id_e = 1
pid_train = np.vstack(( np.ones((500,1)) , np.zeros((500,1)) ))
pid_test  = np.vstack(( np.zeros((500,1)) , np.ones((500,1)) ))

# reshape data to work as input for SVM
X = data_train[:,1:3]
y = np.ravel(pid_train)
h = .001  # step size in the mesh

# prepare and train SVM
C = 1.0  # SVM regularization parameter
svc = svm.SVC(kernel='linear', C=C).fit(X, y)
rbf_svc = svm.SVC(kernel='rbf', gamma=0.7, C=C).fit(X, y)
poly_svc = svm.SVC(kernel='poly', degree=3, C=C).fit(X, y)
lin_svc = svm.LinearSVC(C=C).fit(X, y)

# create a mesh to plot in
x_min, x_max = X[:, 0].min() - 0.01, X[:, 0].max() + 0.01
y_min, y_max = X[:, 1].min() - 0.01, X[:, 1].max() + 0.01
xx, yy = np.meshgrid(np.arange(x_min, x_max, h),
                     np.arange(y_min, y_max, h))

# title for the plots
titles = ['SVC with linear kernel',
          'LinearSVC (linear kernel)',
          'SVC with RBF kernel',
          'SVC with polynomial (degree 3) kernel']

# plot decision boundaries and test data
for i, clf in enumerate((svc, lin_svc, rbf_svc, poly_svc)):
    # Plot the decision boundary. For that, we will assign a color to each
    # point in the mesh [x_min, m_max]x[y_min, y_max].
    plt.subplot(2, 2, i + 1)
    plt.subplots_adjust(wspace=0.4, hspace=0.4)

    Z = clf.predict(np.c_[xx.ravel(), yy.ravel()])

    # Put the result into a color plot
    Z = Z.reshape(xx.shape)
    plt.contourf(xx, yy, Z, cmap=plt.cm.Paired, alpha=0.8)

    # Plot also the training points
    plt.scatter(X[:, 0], X[:, 1], c=y, cmap=plt.cm.Paired)
    plt.xlabel('energy EMCAL')
    plt.ylabel('energy HCAL (in)')
    plt.xlim(xx.min(), xx.max())
    plt.ylim(yy.min(), yy.max())
    plt.xticks(())
    plt.yticks(())
    plt.title(titles[i])

plt.show()

for i, clf in enumerate((svc, lin_svc, rbf_svc, poly_svc)):

    pid_predict = clf.predict( data_test[:,1:3] )

    # check prediction against truth
    count_all = 0
    count_electron_as_electron = 0
    count_pion_as_pion = 0
    count_electron_as_pion = 0
    count_pion_as_electron = 0
    count_true_electron = 0
    count_true_pion = 0
    
    for pid_pred, pid_true in itertools.izip(pid_predict, pid_test):
        
        count_all += 1
        
        if (pid_pred == id_e) & (pid_true == id_e):
            count_electron_as_electron += 1
            count_true_electron += 1
        elif (pid_pred == id_pi) & (pid_true == id_pi):
            count_pion_as_pion += 1
            count_true_pion += 1
        elif (pid_pred == id_pi) & (pid_true == id_e):
            count_electron_as_pion += 1
            count_true_electron += 1
        elif (pid_pred == id_e) & (pid_true == id_pi):
            count_pion_as_electron += 1
            count_true_pion += 1
        else:
            pass

    print "-----------------------------------"
    print clf
    print "-----------------------------------"
    print "Electrons identified as electrons: %d out of %d" % (count_electron_as_electron, count_true_electron)
    print "Pions identified as electrons:     %d out of %d" % (count_pion_as_electron, count_true_pion)
    print "-----------------------------------"
    print "Pions identified as pions:         %d out of %d" % (count_pion_as_pion, count_true_pion)
    print "Electrons identified as pions:     %d out of %d" % (count_electron_as_pion, count_true_electron)
    print "-----------------------------------"

