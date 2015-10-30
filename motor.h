#include <default_gui_model.h>
#include <phidget21.h>

class motor : public DefaultGUIModel {
	
	Q_OBJECT
	
	public:
		motor(void);
		virtual ~motor(void);
	
		void execute(void);
		void createGUI(DefaultGUIModel::variable_t *, int);
		void customizeGUI(void);
	
	protected:
		virtual void update(DefaultGUIModel::update_flags_t);
	
	private:
		double period;
		double velocity;

		void init_motor();

		private slots:
};
