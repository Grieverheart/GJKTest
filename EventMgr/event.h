#ifndef _EVENT_H
#define _EVENT_H

#include <cctype>
#include <cstdint>

//A Hashed String. Stores the string and its hash which is calculated uppon construction
class HashedString{
public:
	explicit HashedString(const char * const text):
		text_(text),
		hash_(calcHash(text))
	{};

	uint32_t getHash(void)const{
		return hash_;
	};
	const char* getText(void)const{
		return text_;
	};
	//Use of the FNV-1a algorithm for hashing
	uint32_t calcHash(const char *text){
		return FNV1aRec(text, FNVoffset);
	};
	
	//Comparison operators for use with set
	bool operator< (const HashedString &other)const{
		return (hash_ < other.getHash()); //possibly use reinterpret cast
	};
	bool operator== (const HashedString &other)const{
		return (hash_ == other.getHash()); //possibly use reinterpret cast
	};
	
private:
	static constexpr uint32_t FNVoffset = 2166136261u;
	static constexpr uint32_t FNVprime 	= 16777619u;

	uint32_t FNV1aRec(const char *text, uint32_t hash){
		return *text == 0 ? hash : FNV1aRec(text + 1, (hash ^ tolower(*text)) * FNVprime);
	}

	const char *text_;
	uint32_t hash_;
};
typedef HashedString EventType;

//An abstract event. Parties should implement their own events as
//they see fit i.e ActorMoved would contain actor id and evtType "actor_moved"
class Event{
public:
	virtual const EventType& getEventType(void)const = 0;
	virtual ~Event(void) = 0;
};

inline Event::~Event(void){}

#endif
