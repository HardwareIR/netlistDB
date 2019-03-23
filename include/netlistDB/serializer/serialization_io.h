#include <iostream>
#include <fstream>
#include <string>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

namespace netlistDB {
namespace serializer {

/*
 * Interface for serializer IO which drives where the output of serializer is placed
 * */
class iSerializationIO {
public:
	std::string FILE_EXTENSION;
	/*
	 * @return actual stream for the serialization output
	 * */
	virtual std::ostream & str() = 0;

	/* set the default file extension for the output files
	 * (if the SerializationIO does not use files this has no effect)
	 * */
	virtual void file_extension(const std::string & extension);

	/*
	 * Notify about change of hierarchy (drill-down)
	 * */
	virtual void hierarchy_push(const std::string & name);
	/*
	 * Notify about change of hierarchy (drill-up)
	 * */
	virtual void hierarchy_pop();
	virtual ~iSerializationIO();
};

/*
 * Serialize everything to specified stream
 * */
class SerializeToStream: public iSerializationIO {
	std::ostream & _str;
public:
	SerializeToStream(std::ostream & str);
	virtual std::ostream & str() override;
};

/*
 * Serialize each module to separate file with optional hierarchy
 * and cleanup
 * */
class SerializeToFiles: public iSerializationIO {
	const boost::filesystem::path root;
	std::vector<std::string> scope;
	bool to_top_dir;
	const bool flat;
	std::ofstream _str;
	bool top_clear;
	bool do_clear_top;
public:
	/*
	 * @param root the folder where the folder with the output files should be created
	 * @param to_top_dir if true all files/folders are stored in $root/$name_of_top
	 *                   if false they are stored in $root/ directly
	 * @param flat if true all files is put in single folder with the name of top
	 * 	if false the files are stored in directories as they are stored in hierarchy of components
	 * @param do_clear_top if true the $root/$name_of_top/ * or $root/ * will be deleted recursively
	 * 	 (depends on to_top_dir argument)
	 * */
	SerializeToFiles(const std::string & root, bool to_top_dir = true,
			bool flat = true, bool do_clear_top = true);
	virtual std::ostream & str() override;
	virtual void hierarchy_push(const std::string & name) override;
	virtual void hierarchy_pop() override;
	virtual ~SerializeToFiles() override;
};

}
}
