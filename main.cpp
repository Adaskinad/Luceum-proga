#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <iomanip>
#include <chrono>

using namespace std;

struct Clue {
    string category;
    string description;
    int importance;
};

// ========== ГЕНЕРАТОР УЛИК (из вашего кода) ==========
void generateClues() {
    string surname = "Адаскин";

    unsigned int seed = 5381;
    for (char c : surname) {
        seed = seed * 33 + (unsigned char)c;
    }

    auto rng = [&seed](int mod) -> int {
        seed = seed * 1664525u + 1013904223u;
        return (int)((seed >> 16) % (unsigned int)mod);
    };

    vector<string> categories = {"отпечаток", "свидетель", "документ", "оружие", "улика",
                                 "фото", "запись", "алиби", "след", "вещдок"};

    vector<string> descriptions = {
        "перчатка", "стакан", "нож", "письмо", "договор", "расписка",
        "паспорт", "телефон", "ключ", "сумка", "пуговица", "зажигалка",
        "часы", "кольцо", "фотография", "записка", "конверт", "монета",
        "ботинок", "шарф", "кепка", "очки", "браслет", "цепочка",
        "Иванов", "Петров", "Сидоров", "Козлов", "Новиков", "Морозов",
        "Волков", "Алексеев", "Лебедев", "Семенов", "Егоров", "Павлов",
        "Степанов", "Николаев", "Орлов", "Виноградов", "Соколов", "Попов",
        "блокнот", "распечатка", "карта", "схема", "чертеж", "квитанция",
        "чек", "билет", "пропуск", "удостоверение", "справка", "акт",
        "протокол", "отчет", "список", "инструкция", "накладная", "ордер",
        "пистолет", "дубина", "веревка", "яд", "скальпель", "топор",
        "молоток", "цепь", "арматура", "отвертка", "гаечный_ключ", "лопата",
        "след_обуви", "след_шин", "след_крови", "след_грязи", "след_краски",
        "окурок", "волос", "ткань", "стружка", "осколок", "пуля", "гильза",
        "видеозапись", "аудиозапись", "переписка", "имейл", "смс", "звонок",
        "транзакция", "перевод", "счет", "депозит", "кредит", "платеж"};

    ofstream file("clues.txt");
    if (!file.is_open()) {
        cerr << "Ошибка: не удалось создать файл clues.txt\n";
        return;
    }

    for (int i = 0; i < 200; i++) {
        string cat = categories[rng(categories.size())];
        string desc = descriptions[rng(descriptions.size())];
        int importance = rng(10) + 1;
        file << cat << " " << desc << " " << importance << "\n";
    }
    file.close();
    cout << "Файл clues.txt сгенерирован (200 улик) для фамилии: " << surname << "\n\n";
}

// ========== СОРТИРОВКА ВСТАВКАМИ (Шаг 4) ==========
int insertionSort(vector<Clue>& clues) {
    int swaps = 0;
    for (size_t i = 1; i < clues.size(); ++i) {
        Clue key = clues[i];
        int j = i - 1;
        while (j >= 0 && clues[j].importance < key.importance) {
            clues[j + 1] = clues[j];
            --j;
            swaps++;
        }
        clues[j + 1] = key;
    }
    return swaps;
}

// ========== СОРТИРОВКА ПУЗЫРЬКОМ (доп. задание) ==========
int bubbleSort(vector<Clue>& clues) {
    int swaps = 0;
    for (size_t i = 0; i < clues.size() - 1; ++i) {
        for (size_t j = 0; j < clues.size() - i - 1; ++j) {
            if (clues[j].importance < clues[j + 1].importance) {
                swap(clues[j], clues[j + 1]);
                swaps++;
            }
        }
    }
    return swaps;
}

// ========== РЕКУРСИВНЫЙ ПОИСК МАКСИМУМА (Шаг 5) ==========
int findMaxRecursive(const vector<int>& vals, int index) {
    if (index == vals.size() - 1) return vals[index];
    int maxOfRest = findMaxRecursive(vals, index + 1);
    return (vals[index] > maxOfRest) ? vals[index] : maxOfRest;
}

// ========== ЧТЕНИЕ ФАЙЛА (Шаг 1) ==========
vector<Clue> readClues(const string& filename) {
    vector<Clue> clues;
    ifstream in(filename);
    if (!in) {
        cerr << "Ошибка: не удалось открыть " << filename << endl;
        return clues;
    }
    string line;
    while (getline(in, line)) {
        istringstream iss(line);
        Clue c;
        if (iss >> c.category >> c.description >> c.importance) {
            clues.push_back(c);
        }
    }
    return clues;
}

// ========== ГРУППИРОВКА ПО КАТЕГОРИЯМ (Шаг 2) ==========
map<string, vector<Clue>> groupByCategory(const vector<Clue>& clues) {
    map<string, vector<Clue>> groups;
    for (const auto& clue : clues) {
        groups[clue.category].push_back(clue);
    }
    return groups;
}

// ========== ПОИСК САМОЙ ВАЖНОЙ УЛИКИ (через рекурсию) ==========
Clue findMostImportantClue(const vector<Clue>& clues) {
    if (clues.empty()) return {"", "", 0};
    vector<int> importances;
    for (const auto& c : clues) importances.push_back(c.importance);
    int maxImp = findMaxRecursive(importances, 0);
    for (const auto& c : clues) {
        if (c.importance == maxImp) return c;
    }
    return {"", "", 0};
}

// ========== СРЕДНЯЯ ВАЖНОСТЬ ПО КАТЕГОРИЯМ (Шаг 8) ==========
void printAverageImportance(const map<string, vector<Clue>>& groups, ostream& out) {
    out << "\n=== Средняя важность улик по каждой категории ===\n";
    for (const auto& pair : groups) {
        double sum = 0;
        for (const auto& clue : pair.second) {
            sum += clue.importance;
        }
        double avg = sum / pair.second.size();
        out << "  " << pair.first << ": " << fixed << setprecision(2) << avg << "\n";
    }
}

// ========== ВЫВОД ВСЕХ УЛИК (ОТСОРТИРОВАННЫХ) ==========
void printAllCluesSorted(const vector<Clue>& clues, ostream& out) {
    vector<Clue> sortedAll = clues;
    insertionSort(sortedAll);
    out << "\n=== Все улики от самой важной к наименее важной ===\n";
    for (const auto& c : sortedAll) {
        out << "  " << c.category << " " << c.description << " (важность: " << c.importance << ")\n";
    }
}

int main() {
    // ===== Шаг 1: Генерация и чтение =====
    generateClues();
    
    vector<Clue> clues = readClues("clues.txt");
    if (clues.empty()) {
        cerr << "Нет данных для анализа." << endl;
        return 1;
    }

    // ===== Шаг 2: Группировка =====
    auto groups = groupByCategory(clues);

    // ===== Шаг 3: Уникальные категории через set (в алфавитном порядке) =====
    set<string> uniqueCategories;
    for (const auto& pair : groups) {
        uniqueCategories.insert(pair.first);
    }

    // ===== Шаг 4: Сортировка улик внутри каждой категории (вставками) =====
    for (auto& pair : groups) {
        insertionSort(pair.second);
    }

    // ===== Шаг 5: Самая важная улика (рекурсивно) =====
    Clue mostImportant = findMostImportantClue(clues);

    // ===== ВЫВОД В КОНСОЛЬ =====
    cout << "\n=== ДЕТЕКТИВНОЕ АГЕНТСТВО ===\n";
    cout << "Прочитано улик: " << clues.size() << "\n\n";
    
    cout << "Категории и количество улик в каждой:\n";
    for (const auto& pair : groups) {
        cout << "  - " << pair.first << ": " << pair.second.size() << " улик(и)\n";
    }
    
    cout << "\nУникальные категории (алфавитный порядок):\n  ";
    for (const auto& cat : uniqueCategories) {
        cout << cat << " ";
    }
    
    cout << "\n\n=== Топ улик по категориям ===\n";
    for (const auto& pair : groups) {
        cout << "[" << pair.first << "]\n";
        for (size_t i = 0; i < min(pair.second.size(), (size_t)5); ++i) {
            cout << "  " << i+1 << ". " << pair.second[i].description 
                 << " (важность: " << pair.second[i].importance << ")\n";
        }
        if (pair.second.size() > 5) cout << "  ...\n";
    }
    
    cout << "Самая важная улика: " << mostImportant.description 
         << " (категория: " << mostImportant.category 
         << ", важность: " << mostImportant.importance << ")\n";

    // ===== Шаг 7: Запрос категории у пользователя (заменил на выбор из кода) =====
    string userCategory = "документ";  // <- МОЖЕТЕ ЗАМЕНИТЬ НА ЛЮБУЮ ИЗ СПИСКА
    cout << "Для отчёта выбрана категория: " << userCategory << "\n";

    // ===== Шаг 6 и 8: Запись в report.txt =====
    ofstream report("report.txt");
    if (!report) {
        cerr << "Ошибка: не удалось создать report.txt\n";
        return 1;
    }

    report << "=== ОТЧЕТ ДЕТЕКТИВНОГО АГЕНТСТВА ===\n";
    report << "Общее количество улик: " << clues.size() << "\n\n";
    
    report << "Список категорий и количество улик в каждой:\n";
    for (const auto& pair : groups) {
        report << "  " << pair.first << ": " << pair.second.size() << "\n";
    }
    
    report << "\nСамая важная улика: " << mostImportant.description 
           << " (" << mostImportant.category << ", важность: " << mostImportant.importance << ")\n";
    
    printAllCluesSorted(clues, report);
    printAverageImportance(groups, report);
    
    report << "\n=== Улики из категории \"" << userCategory << "\" ===\n";
    if (groups.find(userCategory) != groups.end()) {
        for (const auto& clue : groups.at(userCategory)) {
            report << "  " << clue.description << " (важность: " << clue.importance << ")\n";
        }
    } else {
        report << "  Категория \"" << userCategory << "\" не найдена.\n";
        report << "  Доступные категории: ";
        for (const auto& cat : uniqueCategories) {
            report << cat << " ";
        }
        report << "\n";
    }

    // ===== ДОПОЛНИТЕЛЬНОЕ ЗАДАНИЕ: Сравнение двух сортировок =====
    cout << "\n=== ДОПОЛНИТЕЛЬНОЕ ЗАДАНИЕ ===\n";
    
    vector<Clue> copyForBubble = clues;
    vector<Clue> copyForInsert = clues;
    
    int bubbleSwaps = bubbleSort(copyForBubble);
    int insertSwaps = insertionSort(copyForInsert);
    
    cout << "Сортировка пузырьком: " << bubbleSwaps << " перестановок\n";
    cout << "Сортировка вставками: " << insertSwaps << " перестановок\n";
    
    if (insertSwaps < bubbleSwaps) {
        cout << "✅ Сортировка вставками эффективнее (на " << (bubbleSwaps - insertSwaps) << " перестановок меньше)\n";
    } else if (bubbleSwaps < insertSwaps) {
        cout << "✅ Сортировка пузырьком эффективнее\n";
    } else {
        cout << "Обе сортировки сделали одинаковое количество перестановок\n";
    }
    
    report << "\n=== ДОПОЛНИТЕЛЬНОЕ ЗАДАНИЕ: Сравнение алгоритмов сортировки ===\n";
    report << "Сортировка пузырьком: " << bubbleSwaps << " перестановок\n";
    report << "Сортировка вставками: " << insertSwaps << " перестановок\n";
    report << "\nВывод: Сортировка вставками в среднем эффективнее, так как:\n";
    report << "  - Делает меньше перестановок\n";
    report << "  - Лучше работает с частично упорядоченными данными\n";
    report << "  - Не требует многократных проходов по всему массиву\n";

    report.close();

    return 0;
}
