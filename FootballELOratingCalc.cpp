#include <iostream>
#include <fstream>
#include <cmath>
#include <map>
#include <string>

using namespace std;

struct Club {
    int id;
    int rating;
    int num_games;
    double match_factor = 20.0;
};

double halfSeries(int n){
    double result = 0;

    if(n<=0){
        result = 0;
    } else {
        double diff = 0.5;
        for(int i = 0; i < n; i++){
            result += diff;
            diff /= 2;
        }
    }

    return result;
}

double goalFactor(int goal_difference){
    double result = 1;
    
    if(goal_difference > 0) result += halfSeries(goal_difference);

    return result;
}

double matchFactor(int game_num, double old_mf){
    if(game_num == 0) return old_mf;
    return old_mf - (1/game_num);
}

double predictScore(Club player, Club opponent){
    int diff = opponent.rating-player.rating;
    return 1.0/(pow(10.0,diff/400)+1);
}

double realScore(int goal_difference){
    double result;

    if(goal_difference > 0){
        result = 1.0;
    } else if(goal_difference < 0){
        result = 0;
    } else {
        result = 0.5;
    }

    return result;
}

void playMatch(Club& home, Club& away, int goal_difference){
    int res = realScore(goal_difference);
    double gf = goalFactor(abs(goal_difference));

    double home_mf = matchFactor(home.num_games, home.match_factor);

    double away_mf = matchFactor(away.num_games, away.match_factor);

    double home_pr_score = predictScore(home, away);
    double away_pr_score = predictScore(away, home);

    int home_diff = round(home_mf*gf*(res-home_pr_score));
    int away_diff = round(away_mf*gf*(1-res-away_pr_score));

    home.rating += home_diff;
    away.rating += away_diff;
    home.num_games++;
    away.num_games++;
    home.match_factor = home_mf;
    away.match_factor = away_mf;
}

bool importDB(string path, map<int,Club> &db ){
    ifstream file(path);
    string header, id, rating, num_games, match_factor;

    if(!file){
        cout<< "Couldn't open a file with given path!" << endl;
        return false;
    }

    getline(file,header);

    while (file.good()){
        getline(file,id,',');
        if(id == "") break;
        getline(file,rating,',');
        getline(file,num_games,',');
        getline(file,match_factor, '\n');

        Club cur;
        cur.id = stoi(id);
        cur.rating = stoi(rating);
        cur.num_games = stoi(num_games);
        cur.match_factor = stod(match_factor);

        db.insert({cur.id, cur});   
    }
    
    file.close();

    return true;
}

bool importMatches(string path, map<int,Club> &db){
    ifstream file(path);
    string header, home, hg_num, ag_num, away;

    if(!file){
        cout<< "Couldn't open a file with given path!" << endl;
        return false;
    }

    getline(file,header);

    while (file.good()){
        getline(file,home, ',');
        if(home == "") break;
        getline(file,hg_num,',');
        getline(file,ag_num,',');
        getline(file,away,'\n');

        int goal_difference = stoi(hg_num) - stoi(ag_num);

        playMatch(db.at(stoi(home)),db.at(stoi(away)),goal_difference);   
    }
    
    file.close();

    return true;
}

bool exportDB(string path, map<int,Club> &db){
    ofstream file(path);;

    if(!file){
        cout<< "Couldn't open a file with given path!" << endl;
        return false;
    }

    file.clear();

    file << "id," << "rating," << "games," << "factor\n";

    for(const auto& it : db){
        file << it.second.id << "," << it.second.rating << "," << it.second.num_games << "," << it.second.match_factor << endl;
    }
    
    file.close();

    return true;
}

bool closeDialog(){
    cout << "If you'd like to try again, please insert Y, otherwise any letter to finish the program running: ";
    string ans;
    cin >> ans;

    if(ans == "Y" || ans == "y") return false;

    return true;
}

string handleDB(map<int,Club> &clubs_db) {
    string db_path;
    cout << "Please enter clubs' database file path: ";
    cin >> db_path;
    while(!importDB(db_path, clubs_db)){
        if(closeDialog()){
            return "";
        } else {
            cout << "Please enter correct clubs' database file path: ";
            cin >> db_path;
        }
    }
    cout << "Clubs' data imported!" << endl;
    return db_path;
}

bool handleMatches(map<int,Club> &clubs_db) {
    int n;
    string path;
    cout << "Please, enter the number of files to import: ";
    cin >> n;
    for(int i = 0; i < n; i++){
        cout << "Please enter match results' file path: ";
        cin >> path;
        while(!importMatches(path, clubs_db)){
            if(closeDialog()){
                return false;
            } else {
                cout << "Please enter correct match results' file path: ";
                cin >> path;
            }
        }
        cout << "Match results of file " << path << " are imported!" << endl;
    }
    return true;
}

void handleExport(map<int,Club> &clubs_db, string db_path){
    cout << "Exporting results to the clubs' database!" << endl;
    if(exportDB(db_path,clubs_db)) {
        cout << "Export finished!" << endl;
    } else {
        cout << "Export failed!" << endl;
    }
}

int main(){
    map <int, Club> clubs_db;
    string db_path;
    bool runApp = true;
    bool result_gate = true;

    while(runApp){
        db_path = handleDB(clubs_db);
        if(db_path == "") break;

        result_gate = handleMatches(clubs_db);
        if(!result_gate) break;

        handleExport(clubs_db, db_path);

        if(closeDialog()) runApp = false;
    }

    return 0;
}