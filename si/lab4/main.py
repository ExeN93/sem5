import numpy as np
import pandas as pd
from sklearn.model_selection import train_test_split
from sklearn.naive_bayes import GaussianNB
from sklearn.metrics import confusion_matrix 
from sklearn.metrics import accuracy_score
from sklearn.datasets import load_iris
import math

def load_data(filename):
    data = []
    with open(filename, 'r') as f:
        for line in f:
            values = line.strip().split('\t')
            values = [float(v) for v in values if v.strip() != '']
            if len(values) > 0:
                data.append(values)
    return np.array(data)

class NaiveBayesCiagly:
    def __init__(self, use_log=False):
        self.use_log = use_log
        self.classes = None
        self.prior_probs = {}
        self.means = {}
        self.stds = {}
        
    def fit(self, X, y):
        self.classes = np.unique(y)
        n_samples = len(y)
        
        for c in self.classes:
            self.prior_probs[c] = np.sum(y == c) / n_samples
            
        for c in self.classes:
            X_c = X[y == c]
            self.means[c] = np.mean(X_c, axis=0)
            self.stds[c] = np.std(X_c, axis=0, ddof=1)
            
    def gaussian_probability(self, x, mean, std):
        if std == 0:
            std = 1e-6  # zapobiega dzieleniu przez 0
        exponent = math.exp(-((x - mean) ** 2) / (2 * std ** 2))
        return (1 / (math.sqrt(2 * math.pi) * std)) * exponent
    
    def predict_probability(self, x):
        probs = {}
        
        if self.use_log:
            for c in self.classes:
                log_prob = math.log(self.prior_probs[c])
                for i in range(len(x)):
                    p = self.gaussian_probability(x[i], self.means[c][i], self.stds[c][i])
                    if p > 0:
                        log_prob += math.log(p)
                    else:
                        log_prob += -1e10  # bardzo mała wartość zamiast -inf
                probs[c] = log_prob
        else:
            # Wersja bez logarytmowania
            for c in self.classes:
                prob = self.prior_probs[c]
                for i in range(len(x)):
                    prob *= self.gaussian_probability(x[i], self.means[c][i], self.stds[c][i])
                probs[c] = prob
        
        if self.use_log:
            max_log = max(probs.values())
            probs_exp = {c: math.exp(p - max_log) for c, p in probs.items()}
            total = sum(probs_exp.values())
            normalized_probs = {c: p / total for c, p in probs_exp.items()}
        else:
            total = sum(probs.values())
            if total == 0:
                normalized_probs = {c: 1.0/len(self.classes) for c in self.classes}
            else:
                normalized_probs = {c: p / total for c, p in probs.items()}
                
        return normalized_probs
    
    def predict(self, X):
        predictions = []
        for x in X:
            probs = self.predict_probability(x)
            pred_class = max(probs, key=probs.get)
            predictions.append(pred_class)
        return np.array(predictions)

def calculate_accuracy(y_true, y_pred):
    correct = np.sum(y_true == y_pred)
    total = len(y_true)
    return (correct / total) * 100

def calculate_accuracy_per_class(y_true, y_pred, classes):
    acc_per_class = {}
    for c in classes:
        mask = y_true == c
        if np.sum(mask) > 0:
            correct = np.sum((y_true == c) & (y_pred == c))
            total = np.sum(mask)
            acc_per_class[c] = (correct / total) * 100
        else:
            acc_per_class[c] = 0
    return acc_per_class

if __name__ == "__main__":
    data = load_data('seeds_dataset.txt')
    X = data[:, :-1]
    y = data[:, -1]
    
    print(f"Liczba próbek: {len(y)}")
    print(f"Liczba cech: {X.shape[1]}")
    print(f"Klasy: {np.unique(y)}")
    print(f"Podział klas: {np.bincount(y.astype(int))[1:]}")
    print()
    
    eksperymenty = []
    
    print("Eksperyment 1: Bez logarytmowania")
    X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.3, random_state=42)
    
    nbc = NaiveBayesCiagly(use_log=False)
    nbc.fit(X_train, y_train)
    
    y_train_pred = nbc.predict(X_train)
    y_test_pred = nbc.predict(X_test)
    
    acc_train = calculate_accuracy(y_train, y_train_pred)
    acc_test = calculate_accuracy(y_test, y_test_pred)
    
    acc_per_class_train = calculate_accuracy_per_class(y_train, y_train_pred, nbc.classes)
    acc_per_class_test = calculate_accuracy_per_class(y_test, y_test_pred, nbc.classes)
    
    print(f"Dokładność na zbiorze treningowym: {acc_train:.2f}%")
    print(f"Dokładność na zbiorze testowym: {acc_test:.2f}%")
    print(f"Accuracy per class: {acc_per_class_test}")
    print()
    
    test_basic_data = []
    for i, (yt, yp) in enumerate(zip(y_test, y_test_pred)):
        probs = nbc.predict_probability(X_test[i])
        correct = 1 if yt == yp else 0
        test_basic_data.append({
            'numer_probki': i,
            'etykieta_prawdziwa': yt,
            'etykieta_przewidziana': yp,
            'poprawnosc': correct,
            'prob_przewidzianej': probs[yp],
            'prob_prawdziwej': probs[yt]
        })
    
    df_basic = pd.DataFrame(test_basic_data)
    df_basic.to_csv('test_basic.csv', index=False)
    
    eksperymenty.append({
        'id_eksperymentu': 'exp1_podstawowy',
        'zbior': 'seeds',
        'wariant': 'ciagly',
        'log': False,
        'proporcja': '70/30',
        'acc_train': acc_train,
        'acc_test': acc_test,
        'acc_class1': acc_per_class_test.get(1.0, 0),
        'acc_class2': acc_per_class_test.get(2.0, 0),
        'acc_class3': acc_per_class_test.get(3.0, 0)
    })

    print("Eksperyment 2: Z logarytmowaniem")
    nbc_log = NaiveBayesCiagly(use_log=True)
    nbc_log.fit(X_train, y_train)
    
    y_train_pred_log = nbc_log.predict(X_train)
    y_test_pred_log = nbc_log.predict(X_test)
    
    acc_train_log = calculate_accuracy(y_train, y_train_pred_log)
    acc_test_log = calculate_accuracy(y_test, y_test_pred_log)
    
    acc_per_class_test_log = calculate_accuracy_per_class(y_test, y_test_pred_log, nbc_log.classes)
    
    print(f"Dokładność na zbiorze treningowym: {acc_train_log:.2f}%")
    print(f"Dokładność na zbiorze testowym: {acc_test_log:.2f}%")
    print(f"Accuracy per class: {acc_per_class_test_log}")
    print()
    
    eksperymenty.append({
        'id_eksperymentu': 'exp2_log',
        'zbior': 'seeds',
        'wariant': 'ciagly',
        'log': True,
        'proporcja': '70/30',
        'acc_train': acc_train_log,
        'acc_test': acc_test_log,
        'acc_class1': acc_per_class_test_log.get(1.0, 0),
        'acc_class2': acc_per_class_test_log.get(2.0, 0),
        'acc_class3': acc_per_class_test_log.get(3.0, 0)
    })
    
    print("Eksperyment 3: Różne proporcje podziału")
    proporcje = [0.5, 0.4, 0.3, 0.2]
    
    for prop in proporcje:
        X_tr, X_te, y_tr, y_te = train_test_split(X, y, test_size=prop, random_state=42)
        
        nbc_prop = NaiveBayesCiagly(use_log=True)
        nbc_prop.fit(X_tr, y_tr)
        
        y_tr_pred = nbc_prop.predict(X_tr)
        y_te_pred = nbc_prop.predict(X_te)
        
        acc_tr = calculate_accuracy(y_tr, y_tr_pred)
        acc_te = calculate_accuracy(y_te, y_te_pred)
        
        acc_per_class_te = calculate_accuracy_per_class(y_te, y_te_pred, nbc_prop.classes)
        
        print(f"Proporcja {int((1-prop)*100)}/{int(prop*100)}: Train={acc_tr:.2f}%, Test={acc_te:.2f}%")
        
        eksperymenty.append({
            'id_eksperymentu': f'exp3_prop_{int((1-prop)*100)}_{int(prop*100)}',
            'zbior': 'seeds',
            'wariant': 'ciagly',
            'log': True,
            'proporcja': f'{int((1-prop)*100)}/{int(prop*100)}',
            'acc_train': acc_tr,
            'acc_test': acc_te,
            'acc_class1': acc_per_class_te.get(1.0, 0),
            'acc_class2': acc_per_class_te.get(2.0, 0),
            'acc_class3': acc_per_class_te.get(3.0, 0)
        })
    print()
    
    print("Eksperyment 4: Porównanie z sklearn")
    X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.3, random_state=42)
    
    sklearn_nbc = GaussianNB()
    sklearn_nbc.fit(X_train, y_train)
    
    y_train_pred_sk = sklearn_nbc.predict(X_train)
    y_test_pred_sk = sklearn_nbc.predict(X_test)
    
    acc_train_sk = calculate_accuracy(y_train, y_train_pred_sk)
    acc_test_sk = calculate_accuracy(y_test, y_test_pred_sk)
    
    acc_per_class_test_sk = calculate_accuracy_per_class(y_test, y_test_pred_sk, nbc.classes)
    
    print(f"Sklearn Dokładność treningowa: {acc_train_sk:.2f}%")
    print(f"Sklearn Dokładność testowa: {acc_test_sk:.2f}%")
    print(f"Accuracy per class: {acc_per_class_test_sk}")
    print()
    
    eksperymenty.append({
        'id_eksperymentu': 'exp4_sklearn',
        'zbior': 'seeds',
        'wariant': 'ciagly_sklearn',
        'log': 'N/A',
        'proporcja': '70/30',
        'acc_train': acc_train_sk,
        'acc_test': acc_test_sk,
        'acc_class1': acc_per_class_test_sk.get(1.0, 0),
        'acc_class2': acc_per_class_test_sk.get(2.0, 0),
        'acc_class3': acc_per_class_test_sk.get(3.0, 0)
    })
    
    print("Eksperyment 7: Iris")
    
    iris_data = load_iris()
    X_iris = iris_data.data
    y_iris = iris_data.target
    
    print(f"Liczba próbek Iris: {len(y_iris)}")
    print(f"Liczba cech Iris: {X_iris.shape[1]}")
    print(f"Klasy Iris: {np.unique(y_iris)}")
    
    X_train_i, X_test_i, y_train_i, y_test_i = train_test_split(X_iris, y_iris, test_size=0.3, random_state=42)
    
    nbc_iris = NaiveBayesCiagly(use_log=True)
    nbc_iris.fit(X_train_i, y_train_i)
    
    y_train_pred_i = nbc_iris.predict(X_train_i)
    y_test_pred_i = nbc_iris.predict(X_test_i)
    
    acc_train_i = calculate_accuracy(y_train_i, y_train_pred_i)
    acc_test_i = calculate_accuracy(y_test_i, y_test_pred_i)
    
    acc_per_class_test_i = calculate_accuracy_per_class(y_test_i, y_test_pred_i, nbc_iris.classes)
    
    print(f"Iris - Dokładność treningowa: {acc_train_i:.2f}%")
    print(f"Iris - Dokładność testowa: {acc_test_i:.2f}%")
    print(f"Accuracy per class: {acc_per_class_test_i}")
    print()
    
    eksperymenty.append({
        'id_eksperymentu': 'exp7_iris',
        'zbior': 'iris',
        'wariant': 'ciagly',
        'log': True,
        'proporcja': '70/30',
        'acc_train': acc_train_i,
        'acc_test': acc_test_i,
        'acc_class1': acc_per_class_test_i.get(0, 0),
        'acc_class2': acc_per_class_test_i.get(1, 0),
        'acc_class3': acc_per_class_test_i.get(2, 0)
    })
    
    df_eksperymenty = pd.DataFrame(eksperymenty)
    df_eksperymenty.to_csv('eksperymenty.csv', index=False)
    
    print("Zapisano")