 # Load libraries
import pandas
from pandas.tools.plotting import scatter_matrix
import matplotlib.pyplot as plt
from sklearn import model_selection
from sklearn.metrics import classification_report
from sklearn.metrics import confusion_matrix
from sklearn.metrics import accuracy_score
from sklearn.linear_model import LogisticRegression, RidgeClassifier
from sklearn.tree import DecisionTreeClassifier
from sklearn.neighbors import KNeighborsClassifier, RadiusNeighborsClassifier
from sklearn.discriminant_analysis import LinearDiscriminantAnalysis
from sklearn.discriminant_analysis import QuadraticDiscriminantAnalysis
from sklearn.neural_network import MLPClassifier
from sklearn.naive_bayes import GaussianNB, MultinomialNB, BernoulliNB
from sklearn.gaussian_process import GaussianProcessClassifier
from sklearn.gaussian_process.kernels import RBF
from sklearn.svm import SVC
from sklearn.ensemble import RandomForestClassifier, AdaBoostClassifier, ExtraTreesClassifier
from sklearn.cluster import SpectralClustering
from sklearn.mixture import GaussianMixture
from sklearn.cluster import KMeans
from sklearn.tree import DecisionTreeClassifier, ExtraTreeClassifier
from sklearn.calibration import CalibratedClassifierCV
from pandas.tools.plotting import andrews_curves
from pandas.tools.plotting import parallel_coordinates
from pandas.tools.plotting import radviz
import seaborn as sns
import numpy as np

# Load dataset
#url = "https://archive.ics.uci.edu/ml/machine-learning-databases/iris/iris.data"
#names = ['sepal-length', 'sepal-width', 'petal-length', 'petal-width', 'class']
path = "./data/JetSummary_p250_e20_1000events_r05.csv"
#names = ['n_Total', 'n_Above_0p001', 'n_Above_0p01', 'n_Above_0p1', 'n_Above_1', 'n_Above_10', 'eta_avg', 'eta_std', 'phi_avg', 'phi_std', 'Delta_eta_avg', 'Delta_eta_std', 'Delta_phi_avg', 'Delta_phi_std', 'Delta_eta_avg_w', 'Delta_eta_std_w', 'Delta_phi_avg_w', 'Delta_phi_std_w', 'towerenergy_sum','class']
names = ['n_track','charge_tot','eta','vertex','class']
dataset = pandas.read_csv(path, names=names)

# shape
#print((dataset.shape)[0])

# head
#print(dataset.head(20))

# descriptions and visualizations
#print(dataset.describe())
#andrews_curves(dataset, 'class')
#parallel_coordinates(dataset, 'class')
#radviz(dataset, 'class')

# class distribution
#print(dataset.groupby('class').size())

# box and whisker plots
#dataset.plot(kind='box', subplots=True, layout=(19,19), sharex=False, sharey=False)
#plt.show()

# histograms
#dataset.hist()
#plt.show()

# scatter plot matrix
#sns.pairplot(dataset, hue="class")
#plt.show()
#plt.savefig('destination_path.eps', format='eps', dpi=1000)

# Split-out validation dataset
array = dataset.values
#X = array[:,0:19]
#Y = array[:,19]
X = array[:,0:4]
Y = array[:,4]
validation_size = 0.60
seed = 7
X_train, X_validation, Y_train, Y_validation = model_selection.train_test_split(X, Y, test_size=validation_size, random_state=seed)

ding = np.column_stack((X_train,Y_train))
print(ding)
print(ding.dtype)
dong = np.column_stack((X_validation,Y_validation))

#np.savetxt("./data/JetSummary_1000_training.csv", ding, delimiter=", ")
#np.savetxt("./data/JetSummary_1000_validation.csv", dong, fmt='%i, %i, %i, %i, %i, %i, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %s')
#np.savetxt("./data/JetSummary_1000_training.csv", ding, fmt='%i, %i, %i, %i, %i, %i, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %s')


# Test options and evaluation metric
seed = 7
scoring = 'accuracy'

# Spot Check Algorithms
models = []
models.append(('LR', LogisticRegression()))
models.append(('LDA', LinearDiscriminantAnalysis()))
#models.append(('QDA', QuadraticDiscriminantAnalysis()))
models.append(('KNN', KNeighborsClassifier()))
models.append(('CART', DecisionTreeClassifier()))
models.append(('GNB', GaussianNB()))
#models.append(('SVM', SVC()))
models.append(('SVMlin', SVC(kernel="linear", C=0.025)))
#models.append(('SVMpoly', SVC(kernel="poly")))
#models.append(('SVMg2', SVC(gamma=2, C=1)))
#models.append(('Neural', MLPClassifier()))
models.append(('RFC', RandomForestClassifier()))
models.append(('ADA', AdaBoostClassifier(base_estimator=None, n_estimators=50, learning_rate=1.0, algorithm='SAMME.R', random_state=None)))
models.append(('ADA3', AdaBoostClassifier(base_estimator=None, n_estimators=100, learning_rate=0.5, algorithm='SAMME.R', random_state=None)))
#models.append(('EXT', ExtraTreesClassifier()))
#models.append(('RC', RidgeClassifier()))
#models.append(('RNeigh', RadiusNeighborsClassifier(200)))
#models.append(('ccCV', CalibratedClassifierCV()))
#models.append(('DTC', DecisionTreeClassifier()))
#models.append(('ETC', ExtraTreeClassifier()))



# evaluate each model in turn
results = []
names = []
for name, model in models:
	kfold = model_selection.KFold(n_splits=10, random_state=seed)
	cv_results = model_selection.cross_val_score(model, X_train, Y_train, cv=kfold, scoring=scoring)
	results.append(cv_results)
	names.append(name)
	msg = "%s: %f (%f)" % (name, cv_results.mean(), cv_results.std())
	print(msg)

# Compare Algorithms
#fig = plt.figure()
#fig.suptitle('Algorithm Comparison')
#ax = fig.add_subplot(111)
#plt.boxplot(results)
#ax.set_xticklabels(names)

# Make predictions on validation dataset
ada = AdaBoostClassifier()
ada.fit(X_train, Y_train)
predictions = ada.predict(X_validation)

#LR = LogisticRegression(class_weight ={'DIS':3,'tau':1})
#LR = LogisticRegression()
#LR.fit(X_train, Y_train)
#predictions = LR.predict(X_validation)
print(accuracy_score(Y_validation, predictions))
print(confusion_matrix(Y_validation, predictions))
print(classification_report(Y_validation, predictions))


plt.show()
