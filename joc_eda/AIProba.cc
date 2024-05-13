#include "Player.hh"
#include<queue>
#include<set>
#include<vector>
#include<limits>

/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME Salchipapa


struct PLAYER_NAME : public Player {

  /**
   * Factory: returns a new instance of this class.
   * Do not modify this function.
   */
  static Player* factory () {
    return new PLAYER_NAME;
  }

  /**
   * Types and attributes for your player can be defined here.
   */

multimap<int, pair<int,Dir>, greater<int>> movements;
struct AStarNode {
    Pos position;

    Dir dir;
    Pos ancestor;
    // Constructor para facilitar la creación de AStarNode
     AStarNode() : position({0, 0,0}), dir(None), ancestor({0,0,0}) {}

    // Constructor para facilitar la creación de AStarNode
    AStarNode(const Pos& position, Dir dir, Pos an) : position(position), dir(dir), ancestor(an) {}

  
};

int manhattanDistance(const Pos& p1, const Pos& p2) {
    return abs(p1.i - p2.i) + abs(p1.j - p2.j);
}
    
  int dist(const Pos& p1, const Pos& p2) {
        int dx = p1.i - p2.i;
        int dy = p1.j - p2.j;
        
        return dx * dx + dy * dy;
    }
  int closestFuryan(const Pos& myPos) {
    int closestFuryanId = -1;
    int minDistance = numeric_limits<int>::max();
    for(int i = 0; i < 4; ++i){
      if( i != me()){
        vector<int> e_furyans = furyans(i);
        
        

        for (int furyanId : e_furyans) {
          Pos furyanPos = unit(furyanId).pos;
          int distance = dist(myPos, furyanPos);

          if (distance < minDistance) {
            minDistance = distance;
            closestFuryanId = furyanId;
        }
        }
      }
    
    }

    return closestFuryanId;
}

int closestPioneer(const Pos& myPos) {
   int closestFuryanId = -1;
    int minDistance = numeric_limits<int>::max();
    for(int i = 0; i < 4; ++i){
      if( i != me()){
        vector<int> e_furyans = pioneers(i);

        for (int furyanId : e_furyans) {
          Pos furyanPos = unit(furyanId).pos;
          int distance = dist(myPos, furyanPos);

          if (distance < minDistance) {
            minDistance = distance;
            closestFuryanId = furyanId;
        }
        }
      }
    
    }

    return closestFuryanId;
}

int closestEnemy(const Pos& myPos) {
    int closestFuryanId = closestFuryan(myPos);
    int closestPioneerId = closestPioneer(myPos);

    // Choose the closer enemy between Furyan and Pioneer
    if (closestFuryanId != -1 && closestPioneerId != -1) {
        Pos furyanPos = unit(closestFuryanId).pos;
        Pos pioneerPos = unit(closestPioneerId).pos;

        int distanceToFuryan = dist(myPos, furyanPos);
        int distanceToPioneer = dist(myPos, pioneerPos);

        return (distanceToFuryan < distanceToPioneer) ? closestFuryanId : closestPioneerId;
    } else if (closestFuryanId != -1) {
        return closestFuryanId;
    } else {
        return closestPioneerId;
    }
}

  bool isHellhound(const Pos& p){
     for (Dir dir : {Bottom, BR, Right, RT, Top, TL, Left, LB}) {
      
      if(pos_ok(p+dir) && cell(p+dir).type != Rock){
        Cell nextCell = cell(p+dir);
        if(nextCell.id != -1 && unit(nextCell.id).type == Hellhound) return true;
      }
       
     }
     return false;
  }

  bool isEnemy(const Pos& p){
    
    if(cell(p).id != -1 and unit(cell(p).id).player != me() and unit(cell(p).id).type != Hellhound){
      return true;
    }
    return false;
  }
  bool attackable(const Pos& p, const int& u){
    if(cell(p).id != -1 and unit(cell(p).id).player != me() and unit(cell(p).id).type != Hellhound && unit(cell(p).id).health <= unit(u).health) return true;
    else return false;
  }
  bool isMine(const Pos& p){
    if(cell(p).id != -1 and unit(cell(p).id).player == me() ) return true;
    else return false;
  }

  vector<Dir> bfs_pioneer(const Pos& start){
    
    queue<pair<Pos, vector<Dir>>> q;
    set<Pos> visited;
    vector<Dir> res;
    
    int max = 10;
    q.push({start, res});
    res.push_back(Dir(random(0,7)));
    while (!q.empty()) {
        Pos currentPos = q.front().first;
        vector<Dir> currentPath = q.front().second;
        q.pop();
        
        if (  (cell(currentPos).owner != me() and cell(currentPos).type != Elevator)) {
            
            return currentPath;
        }
        if(currentPath.size() >= max) return res;
        visited.insert(currentPos);

        for (int i = 0; i <8; ++i) {
            Pos nextPos = currentPos + (Dir)i;

            if (!visited.count(nextPos) && pos_ok(nextPos) && cell(nextPos).type != Rock &&
                !isEnemy(nextPos) && !isHellhound(nextPos) && !isMine(nextPos)) {
                vector<Dir> nextPath = currentPath;
                nextPath.push_back((Dir)i);
                q.push({nextPos, nextPath});
            }
        }
    }
    return res;
  }

  vector<Dir> astar_furyan(const Pos& start, const Pos& target) {
    multimap<int, AStarNode> openSet;
    map<Pos, AStarNode> visited;
    map<Pos, int> fValues;

    openSet.insert({manhattanDistance(start, target), AStarNode(start,None,start)});
    fValues[start] = manhattanDistance(start, target);

    while (!openSet.empty()) {
        AStarNode currentNode = openSet.begin()->second;
        openSet.erase(openSet.begin());
        visited.insert({currentNode.position,currentNode});

        if (currentNode.position == target) {
            // Reconstruir el camino desde el objetivo hasta el inicio
            vector<Dir> pathfromstart;
            while (currentNode.dir != None) {
                pathfromstart.push_back(currentNode.dir);
                currentNode = visited[currentNode.ancestor];
            }
            reverse(pathfromstart.begin(), pathfromstart.end());
            return pathfromstart;
        }

        for (int i = 0; i < 8; ++i) {
            Pos nextPos = currentNode.position + (Dir)i;

            if (pos_ok(nextPos) && cell(nextPos).type != Rock && !isHellhound(nextPos) && !isMine(nextPos)) {
                int tentative_f = fValues[currentNode.position] + 1 + manhattanDistance(nextPos, target);

                if (fValues.find(nextPos) == fValues.end()) {
                    // Este camino es mejor que los anteriores
                    fValues[nextPos] = tentative_f;
                    openSet.insert({tentative_f, AStarNode(nextPos,(Dir)i,currentNode.position)});
                    
                }
                else if(tentative_f < fValues[nextPos]){
                    int f = fValues[nextPos];
                    auto range = openSet.equal_range(f);
                    for(auto it = range.first; it != range.second; ++it){
                      if((it ->second).position == nextPos){
                        it = openSet.erase(it);
                      }
                    }
                    fValues[nextPos] = tentative_f;
                    openSet.insert({tentative_f, AStarNode(nextPos,(Dir)i,currentNode.position)});
                }
            }
        }
    }

    // Si no se encuentra un camino, devuelve un vector vacío
    return {(Dir)random(0,7)};
    

}


  
  pair <int,Dir> scape(const Dir& enemy, const Pos& currentPos) {
    
    const map<Dir, vector<Dir>> oppositeDirs = {
        {Top, {Bottom, LB, BR,Left,Right,TL,RT}},
        {TL, {BR, Bottom, Right,RT,LB,Top,Left}},
        {RT, {LB, Bottom, Left,TL,BR,Top,Right}},
        {Left, {Right, RT, BR,Top,Bottom,TL,LB}},
        {LB, {RT, Top, Right,BR,TL,Left,Bottom}},
        {BR, {TL, Top, Left,LB,RT,Right,Bottom}},
        {Right, {Left, TL, LB,Top,Bottom,RT,BR}},
        {Bottom, {Top, TL, RT,Left,Right,BR,LB}}
    };

    auto it = oppositeDirs.find(enemy);
    if (it != oppositeDirs.end()) {

        
        const vector<Dir>& escapeDirs = it -> second;

        
        if (!escapeDirs.empty()) {
            for (int i = 0; i <6; ++i) {
                Pos escapePos = currentPos + escapeDirs[i];

                
                if (pos_ok(escapePos) && cell(escapePos).type != Rock ) {
                    if(isMine(escapePos) or (cell(currentPos).id != -1 && unit(cell(currentPos).id).type == Pioneer)){
                      return {0,escapeDirs[i]};
                    }
                    return {5,escapeDirs[i]};
                }
            }
        }
    }

    return {5,Dir(random(0, 7))};
}
  Dir hasEnemyAtDistance(const Pos& start, int distance) {
    for(int  k= 1; k <= distance; ++k){
    for (int i = -k; i <= k; ++i) {
        for (int j = -k; j <= k; ++j) {
             if (abs(i) == k || abs(j) == k){

             
            Pos currentPos = start + Pos(i, j,0);

            if (pos_ok(currentPos) && cell(currentPos).type != Rock) {
                if (cell(currentPos).id != -1) {
                    int unitId = cell(currentPos).id;
                    if (unit(unitId).player != me() && unit(unitId).type == Furyan) {
                        return getDirToPos(start, currentPos);
                    }
                }
                else if(isHellhound(currentPos)){
                  
                  
                  vector<int> H = hellhounds();
                 
                  return getDirToPos(start, currentPos);
                } 
            }
        }
        }
    }
    }
    // If no enemy is found, return None
    return None;
}
  Dir hasEnemyAtDistance_f(const Pos& start, int distance, const int& id) {
    for(int  k= 1; k <= distance; ++k){
    for (int i = -k; i <= k; ++i) {
        for (int j = -k; j <= k; ++j) {
           if (abs(i) == k || abs(j) == k){
            Pos currentPos = start + Pos(i, j,0);

            if (pos_ok(currentPos) && cell(currentPos).type != Rock) {
                if (cell(currentPos).id != -1) {
                    int unitId = cell(currentPos).id;
                    if (unit(unitId).player != me() && !attackable(currentPos,id)) {
                        return getDirToPos(start, currentPos);
                    }
                }
                else if(isHellhound(currentPos)){
                  return getDirToPos(start, currentPos);
                  
                } 
            }
        }
        }
    }
    }
   
    return None;
}
  
  Dir getDirToPos(const Pos& from, const Pos& to) {
    int dy = to.i - from.i;
    int dx = to.j - from.j;
    if (dx == -2 && dy == -2) return TL; // Check for diagonal patterns
    else if (dx == 2 && dy == -2) return RT;
    else if (dx == -2 && dy == 2) return LB;
    else if (dx == 2 && dy == 2) return BR;

    else if (dy == -2) return Top; // Check for patterns in the vertical direction
    else if ( dy == 2) return Bottom;

    else if (dx == -2 ) return Left; // Check for patterns in the horizontal direction
    else if (dx == 2 ) return Right;

    else if (dx == -1 && dy == -1) return TL; // Check for diagonal patterns
    else if (dx == 1 && dy == -1) return RT;
    else if (dx == -1 && dy == 1) return LB;
    else if (dx == 1 && dy == 1) return BR;

    else if (dy == -1) return Top; // Check for patterns in the vertical direction
    else if (dy == 1) return Bottom;

    else if (dx == -1 ) return Left; // Check for patterns in the horizontal direction
    else if (dx == 1 ) return Right;
    

    // If none of the patterns match, return None or any default direction
    return None; // You can choose any default direction for non-matching patterns
}



  void move_pioneers(){
    vector<int> myPioneers = pioneers(me());
    int mida = myPioneers.size();
    for (int i = 0; i < mida ; ++i) {

        Dir res = hasEnemyAtDistance(unit(myPioneers[i]).pos,2);
        if(res == None){
           vector<Dir> path = bfs_pioneer(unit(myPioneers[i]).pos);
           movements.insert({3,{myPioneers[i],path[0]}});
          }
        else{
          pair<int,Dir> result = scape(res,unit(myPioneers[i]).pos);
          if(result.first == 0){
              movements.insert({result.first +1,{myPioneers[i],result.second}});
        }
        else movements.insert({result.first,{myPioneers[i],result.second}});
          
        } 
          }
        
        
}
      
  void move_furyans(){
    vector<int> myFuryans = furyans(me());
    int mida = myFuryans.size();
    int strategy = 0;
    if(mida < 8){
      strategy = 1;
    }
    for (int i = 0; i < mida; ++i) {
      Dir res = hasEnemyAtDistance_f(unit(myFuryans[i]).pos,2,myFuryans[i]);
      
      if(res == None){
          int target;
          if(strategy){
            target = closestEnemy(unit(myFuryans[i]).pos);
          }
            else target = closestEnemy(unit(myFuryans[i]).pos);
          vector<Dir> path  = astar_furyan(unit(myFuryans[i]).pos,unit(target).pos);
          movements.insert({7,{myFuryans[i],path[0]}});
          
        }
      else{
        pair<int,Dir> result = scape(res,unit(myFuryans[i]).pos);
        if(result.first != 0){
          movements.insert({result.first +5,{myFuryans[i],result.second}});
        }
        else movements.insert({result.first,{myFuryans[i],result.second}});
        
      }
      
      
    }
  
  }

    
  /**
   * Play method, invoked once per each round.
   */
  virtual void play () {
    move_furyans();
    
    move_pioneers();
    
    for (auto it = movements.begin(); it != movements.end(); ++it) {
        int key = it->first;                  // Obtener la clave (int)
        pair<int, Dir> value = it->second;    // Obtener el valor (pair<int,Dir>)

       command(value.first,value.second);
    }
  }

};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
