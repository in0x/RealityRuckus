# Things I noticed after looking over our repo / code a month later

### Make code more modular
	Try to make code modules less interconnected. Right now our include structure is a bit of a mess, I couldn't even use "Tile" in the pathfinding test module without having to include 10 other files

### Think about using Design Patterns
	Whenever you start a new module think long and hard if a pattern could make life easier. I.e: Unitmanager should be a factory, Managers should probably be singletons, Unit / Unitmanager should follow the Observer pattern, UI should probably be a MVC

### Make all member functions either capitalized or not, maybe make all public functions lower case (like STL), all private functions Uppercase

### Think about if things should really be public
	A lot of classes have a very long list of only public members and properties. Try to do more encapsulation, if another module really needs access to something, try to create an interface between the two

### We should really be using std::array instead of a C-style array, unless we are interfacing with a C-API

### Commit more, smarter, Push less
	Write more detailed commit logs, they can help us out a lot. Commit more often, but don't push unless someone else should be messing with your code