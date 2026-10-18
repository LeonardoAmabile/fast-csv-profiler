import os
from jinja2 import Environment, FileSystemLoader

def build_report(data: dict, output_path: str, template_name: str = "report_base.html"):
    """
    Takes the statistics dictionary and renders an HTML report using Jinja2.
    """
    # 1. Setup Jinja2 environment (tell it where to find the templates folder)
    # os.path.dirname(__file__) gets the directory where this python file lives
    current_dir = os.path.dirname(os.path.abspath(__file__))
    templates_dir = os.path.join(current_dir, "templates")
    
    env = Environment(loader=FileSystemLoader(templates_dir))
    
    # 2. Load the specific template
    try:
        template = env.get_template(template_name)
    except Exception as e:
        raise FileNotFoundError(f"Could not find template '{template_name}' in {templates_dir}")
    
    # 3. Render the HTML by passing the data dictionary to the template
    html_content = template.render(data=data)
    
    # 4. Save the generated HTML to the requested output path
    with open(output_path, "w", encoding="utf-8") as f:
        f.write(html_content)
        
    print(f"Visual report successfully generated at: {output_path}")